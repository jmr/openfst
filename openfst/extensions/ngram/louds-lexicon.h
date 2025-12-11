// Copyright 2025 The OpenFst Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// See www.openfst.org for extensive documentation on this weighted
// finite-state transducer library.
//
// CompactLexiconFst implements a character-to-word transducer backed by two
// LOUDS trees. The input fst must be a valid minimal, deterministic, unweighted
// character-to-word transducer.
// See "Engineering the LOUDS Succinct Tree Representation" - Delpratt, Rahman,
// Raman for information about the LOUDS tree data structure.
// https://link.springer.com/chapter/10.1007/11764298_12
//
// The input automaton must meet a few constraints:
// 1) Exactly one final state. TODO: Relax this requirement.
// 2) Connected (every state is both accessible and co-accessible).
// 3) Three types of arcs: prefix out-tree/suffix in-tree, bridge, closure.
//     - prefix out-tree and suffix in-tree arcs have epsilon output labels
//     - bridge arcs have non-epsilon input labels
//     - Removing the bridge arcs disconnects the fst and leaves two trees
//       (when not considering the closure arcs)
//     - Closure arcs go from the final to initial state
// 4) Unweighted, possibly closed (arcs going from the only final state to the
//    initial state), but the remaining graph is deterministic and acyclic.
// These properties are checked by HasCompactLexiconStructure.
// The input Fst and the resulting CompactLexiconFst may have different state
// numberings, since LOUDS implicitly orders nodes in breadth first order.

#ifndef OPENFST_EXTENSIONS_NGRAM_LOUDS_LEXICON_H_
#define OPENFST_EXTENSIONS_NGRAM_LOUDS_LEXICON_H_

#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <ios>
#include <istream>
#include <iterator>
#include <limits>
#include <memory>
#include <ostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/memory/memory.h"
#include "openfst/compat/compat_memory.h"
#include "openfst/extensions/ngram/bitmap-index.h"
#include "openfst/extensions/ngram/louds-tree.h"
#include "openfst/lib/cache.h"
#include "openfst/lib/compact-fst.h"
#include "openfst/lib/fst.h"
#include "openfst/lib/properties.h"
#include "openfst/lib/util.h"
#include "util/coding/shrunk-array.h"

namespace fst {

// We do not compact the reverse labels other than using the smallest integer
// type that can hold all ilabels. This type is passed to SuffixArcILabel.
// We then only store the ilabel for that arc.
// We note that we don't use the same shrunk array for suffix in-tree labels as
// the prefix out-tree labels since there is no guarantee that they will come
// from a similar distribution. This is a potential future optimization.
template <class Arc, class SuffixArcILabel>
class LexiconElementCompactor {
 public:
  using Weight = typename Arc::Weight;
  using Label = typename Arc::Label;
  using StateId = typename Arc::StateId;

  using ArcElement = Label;
  using SuffixArcElement = SuffixArcILabel;

  LexiconElementCompactor() = default;

  ArcElement CompactTreeArc(const Arc &arc) const { return arc.ilabel; }

  Arc GetArc(const ArcElement &element, StateId nextstate) const {
    return Arc(element, 0, Weight::One(), nextstate);
  }

  Arc GetReverseArc(const SuffixArcElement &element, StateId nextstate) const {
    return Arc(element, 0, Weight::One(), nextstate);
  }

  Arc GetBridgeArc(uint64_t i, uint64_t o, uint64_t n, StateId offset) const {
    return Arc(i, o, Weight::One(), n + offset);
  }

  static const std::string &Type() {
    std::string type = "lexicon-element-";
    type += std::to_string(CHAR_BIT * sizeof(SuffixArcILabel));
    static const std::string *const out = new std::string("lexicon-element-");
    return *out;
  }

  bool IsCompatible(const Fst<Arc> &fst) const { return true; }

  constexpr uint64_t Properties() const { return 0; }
};

// The LexiconCompactor implements the new interface described in
// compact-fst.h. The LOUDS tree topology and the
// arc weights/labels and final weights are stored in this class. The weights
// and labels are compacted/expanded via the LexiconElementCompactor.
template <class Arc, class SuffixILabel>
class LexiconCompactor {
 public:
  using StateId = typename Arc::StateId;
  using Label = typename Arc::Label;
  using Weight = typename Arc::Weight;
  using ElementCompactor = LexiconElementCompactor<Arc, SuffixILabel>;
  using ArcElement = typename ElementCompactor::ArcElement;
  using SuffixArcElement = typename ElementCompactor::SuffixArcElement;

  LexiconCompactor() = default;

  // Compactor interface requires a copy constructor making a thread-safe
  // copy.
  // TODO: This currently does a deep copy. Replace large
  // objects with shared_ptrs where appropriate.
  LexiconCompactor(const LexiconCompactor &other)
      : prefix_out_tree_(other.prefix_out_tree_),
        suffix_in_tree_(other.suffix_in_tree_),
        reverse_arcs_(other.reverse_arcs_),

        prefix_out_tree_shrunk_size_(other.prefix_out_tree_shrunk_size_),
        ilabel_shrunk_size_(other.ilabel_shrunk_size_),
        olabel_shrunk_size_(other.olabel_shrunk_size_),
        next_state_shrunk_size_(other.next_state_shrunk_size_),

        prefix_out_tree_array_(CopyArray(other.prefix_out_tree_array_.get(),
                                         prefix_out_tree_shrunk_size_)),
        ilabel_array_(
            CopyArray(other.ilabel_array_.get(), ilabel_shrunk_size_)),
        olabel_array_(
            CopyArray(other.olabel_array_.get(), olabel_shrunk_size_)),
        next_state_array_(
            CopyArray(other.next_state_array_.get(), next_state_shrunk_size_)),

        prefix_out_tree_reader_(ShrunkArray::Reader::New()),
        ilabel_reader_(ShrunkArray::Reader::New()),
        olabel_reader_(ShrunkArray::Reader::New()),
        next_state_reader_(ShrunkArray::Reader::New()),

        prefix_out_tree_decode_key_(other.prefix_out_tree_decode_key_),
        ilabel_decode_key_(other.ilabel_decode_key_),
        olabel_decode_key_(other.olabel_decode_key_),
        next_state_decode_key_(other.next_state_decode_key_),

        num_bridge_arcs_(other.num_bridge_arcs_),
        bm_(CopyArray(other.bm_.get(), other.bridge_arc_bitmap_.ArraySize())),
        closure_arcs_(other.closure_arcs_) {
    prefix_out_tree_reader_->Bind(prefix_out_tree_array_.get(),
                                  prefix_out_tree_decode_key_.data());
    ilabel_reader_->Bind(ilabel_array_.get(), ilabel_decode_key_.data());
    olabel_reader_->Bind(olabel_array_.get(), olabel_decode_key_.data());
    next_state_reader_->Bind(next_state_array_.get(),
                             next_state_decode_key_.data());

    bridge_arc_bitmap_.BuildIndex(
        bm_.get(), prefix_out_tree_.NumNodes() + num_bridge_arcs_);
  }

  LexiconCompactor(LexiconCompactor &&) = default;

  // The following is an overview of how this constructor works.
  // The general idea is the following: all the states in the prefix out-tree
  // will be labeled [0, n) by breadth first order (making it ordinal)
  // then all nodes in the suffix in-tree will be labeled [n, n+m) in breadth
  // first order except we are traversing using the _reverse_ of each arc. We
  // first find the closure arcs (if any). That is, the arcs leading from the
  // only final state to the only initial state. Afterwards, we only deal with
  // the underlying acyclic graph.

  // 1) get all of the states in the prefix out-tree (nodes that appear before
  // bridge arcs)
  //     - In this step we also do the following things
  //       1) get the bridge arcs beginning at each state
  //       2) get the number of children (nodes that are reached by non-bridge
  //          arcs) for each state
  //       3) compact and store the non-bridge arcs
  //       4) map each StateId to a unique stateid in the range [0, n) based on
  //          the order they are visited (BFS)
  // 2) get all of the arcs in the suffix in-tree
  //    - Basically, we make a map<StateId, vector<Arc>> where the vector holds
  //      all arcs ending at StateId. Then, we just find the final state
  //      (there is only one) and do reverse BFS from it
  //    - In this step we also
  //      1) map each suffix in-tree state to the range [n, n+m) based on their
  //         position in (reverse) BFS order
  //      2) compact and store the reverse arcs
  //      3) get the number of children for each state (here, a child in the
  //         suffix in-tree has an arc that leads to the parent)
  // 3) rebuild and store each bridge arc
  //    - We use the suffix in-tree node stateid mapping to fix the nextstate
  //      for each bridge arc

  // All of this is necessary because the states of the input fst don't have to
  // follow any ordering that differentiates prefix out-tree from suffix in-tree
  // nodes.

  // TODO: Figure out if the LexiconCompactor arg really needs to
  // be supported.
  explicit LexiconCompactor(const Fst<Arc> &fst,
                            std::shared_ptr<LexiconCompactor> = nullptr) {
    size_t num_states = 0;
    StateId final_state = kNoStateId;
    for (StateIterator<Fst<Arc>> siter(fst); !siter.Done(); siter.Next()) {
      const StateId state = siter.Value();
      num_states++;
      if (fst.Final(state) != Weight::Zero()) {
        final_state = state;
      }
    }
    if (final_state != kNoStateId) {
      for (ArcIterator<Fst<Arc>> aiter(fst, final_state); !aiter.Done();
           aiter.Next()) {
        const auto arc = aiter.Value();
        closure_arcs_.emplace_back(arc.ilabel, arc.olabel, arc.weight, 0);
      }
    }
    // Builds the prefix out-tree and record the number of bridge arcs.
    std::vector<size_t> degreeseq;
    size_t num_bridge_arcs = 0;
    if (fst.Start() != kNoStateId) {
      std::queue<StateId> q({fst.Start()});
      while (!q.empty()) {
        const StateId state = q.front();
        q.pop();
        size_t num_tree_arcs = 0;
        for (ArcIterator<Fst<Arc>> aiter(fst, state); !aiter.Done();
             aiter.Next()) {
          const auto arc = aiter.Value();
          if (arc.olabel != 0) {  // Bridge arc
            ++num_bridge_arcs;
          } else {
            q.push(arc.nextstate);
            ++num_tree_arcs;
          }
        }
        degreeseq.push_back(num_tree_arcs);
      }
    }
    prefix_out_tree_.Init(degreeseq);
    const size_t n =
        BitmapIndex::StorageSize(prefix_out_tree_.NumNodes() + num_bridge_arcs);
    bm_ = std::make_unique<uint64_t[]>(n);
    // These will hold the arcs before they are compacted and/or relabeled.
    std::vector<Arc> arcs, bridge_arcs;
    StateId nextstate = 1;
    absl::flat_hash_set<StateId> prefix_out_tree_states;
    // Reiterates over prefix out-tree to build bridge arc bitmap and stores
    // the prefix out-tree and bridge arcs.
    if (fst.Start() != kNoStateId) {
      std::queue<StateId> q({fst.Start()});
      size_t index = 0;
      while (!q.empty()) {
        const StateId state = q.front();
        q.pop();
        prefix_out_tree_states.insert(state);
        for (ArcIterator<Fst<Arc>> aiter(fst, state); !aiter.Done();
             aiter.Next()) {
          const auto &arc = aiter.Value();
          if (arc.olabel != 0) {  // Bridge arc.
            // Stores the bridge arcs until we can get the correct suffix
            // in-tree stateid.
            bridge_arcs.push_back(arc);
            BitmapIndex::Set(bm_.get(), index++);
          } else {
            q.push(arc.nextstate);
            // Relabels the prefix out-tree to be labeled in BFS order.
            arcs.emplace_back(arc.ilabel, arc.olabel, arc.weight, nextstate++);
          }
        }
        BitmapIndex::Clear(bm_.get(), index++);
      }
    }
    bridge_arc_bitmap_.BuildIndex(
        bm_.get(), prefix_out_tree_.NumNodes() + num_bridge_arcs);
    // Builds the suffix in-tree tree.
    const StateId offset = prefix_out_tree_.NumNodes();
    absl::flat_hash_map<StateId, std::vector<Arc>> bridge_arcs_ending_at;
    for (StateIterator<Fst<Arc>> siter(fst); !siter.Done(); siter.Next()) {
      const StateId state = siter.Value();
      if (state != final_state &&
          // Not the root of the suffix in-tree and not in the prefix out-tree.
          prefix_out_tree_states.count(state) == 0) {
        for (ArcIterator<Fst<Arc>> aiter(fst, state); !aiter.Done();
             aiter.Next()) {
          const auto &arc = aiter.Value();
          bridge_arcs_ending_at[arc.nextstate].emplace_back(
              arc.ilabel, arc.olabel, arc.weight, state);
        }
      }
    }
    // Here we are relabelling the suffix in-tree.
    degreeseq.clear();
    absl::flat_hash_map<StateId, StateId> suffix_in_tree_state_map;
    StateId statenum = 0;
    if (final_state != kNoStateId) {
      std::queue<StateId> q({final_state});
      while (!q.empty()) {
        const StateId state = q.front();
        q.pop();
        suffix_in_tree_state_map[state] = statenum + offset;
        size_t num_tree_arcs = 0;
        const auto i = bridge_arcs_ending_at.find(state);
        if (i != bridge_arcs_ending_at.end()) {
          // There are some arcs in the suffix in-tree going to state.
          num_tree_arcs = i->second.size();
          for (auto arc : i->second) {
            q.push(arc.nextstate);
            reverse_arcs_.push_back(element_compactor_.CompactTreeArc(arc));
          }
        }
        degreeseq.push_back(num_tree_arcs);
        ++statenum;
      }
    }
    suffix_in_tree_.Init(degreeseq);
    auto prefix_out_tree_array =
        make_unique_for_overwrite<uint64_t[]>(arcs.size());
    for (int i = 0; i < arcs.size(); i++) {
      prefix_out_tree_array[i] = element_compactor_.CompactTreeArc(arcs[i]);
    }
    // Compresses the prefix out-tree arcs.
    std::vector<uint64_t> prefix_out_tree_vec;
    ShrunkArray::Write(
        prefix_out_tree_array.get(), arcs.size(), kShrunkArrayComplexity,
        prefix_out_tree_decode_key_.data(), &prefix_out_tree_vec);
    prefix_out_tree_array_.reset(ShrunkArray::Copy(prefix_out_tree_vec));
    prefix_out_tree_shrunk_size_ = prefix_out_tree_vec.size();

    prefix_out_tree_reader_.reset(ShrunkArray::Reader::New());
    prefix_out_tree_reader_->Bind(prefix_out_tree_array_.get(),
                                  prefix_out_tree_decode_key_.data());
    // Relabels and compresses the bridge arcs.
    num_bridge_arcs_ = bridge_arcs.size();
    auto ilabel_array =
        make_unique_for_overwrite<uint64_t[]>(bridge_arcs.size());
    auto olabel_array =
        make_unique_for_overwrite<uint64_t[]>(bridge_arcs.size());
    auto next_state_array =
        make_unique_for_overwrite<uint64_t[]>(bridge_arcs.size());
    for (int i = 0; i < bridge_arcs.size(); ++i) {
      const auto &arc = bridge_arcs[i];
      ilabel_array[i] = arc.ilabel;
      olabel_array[i] = arc.olabel;
      next_state_array[i] =
          suffix_in_tree_state_map[arc.nextstate] - prefix_out_tree_.NumNodes();
    }
    std::vector<uint64_t> ilabel_vec;
    ShrunkArray::Write(ilabel_array.get(), bridge_arcs.size(),
                       kShrunkArrayComplexity, ilabel_decode_key_.data(),
                       &ilabel_vec);
    ilabel_array_.reset(ShrunkArray::Copy(ilabel_vec));
    ilabel_shrunk_size_ = ilabel_vec.size();
    ilabel_reader_.reset(ShrunkArray::Reader::New());
    ilabel_reader_->Bind(ilabel_array_.get(), ilabel_decode_key_.data());
    std::vector<uint64_t> olabel_vec;
    ShrunkArray::Write(olabel_array.get(), bridge_arcs.size(),
                       kShrunkArrayComplexity, olabel_decode_key_.data(),
                       &olabel_vec);
    olabel_array_.reset(ShrunkArray::Copy(olabel_vec));
    olabel_shrunk_size_ = olabel_vec.size();
    olabel_reader_.reset(ShrunkArray::Reader::New());
    olabel_reader_->Bind(olabel_array_.get(), olabel_decode_key_.data());
    std::vector<uint64_t> next_state_vec;
    ShrunkArray::Write(next_state_array.get(), bridge_arcs.size(),
                       kShrunkArrayComplexity, next_state_decode_key_.data(),
                       &next_state_vec);
    next_state_array_.reset(ShrunkArray::Copy(next_state_vec));
    next_state_shrunk_size_ = next_state_vec.size();
    next_state_reader_.reset(ShrunkArray::Reader::New());
    next_state_reader_->Bind(next_state_array_.get(),
                             next_state_decode_key_.data());
  }

  // Each lexicon state has a stateid and a tree_state_id. The stateid is the
  // global stateid while the tree_state_id_ is the id within the in- or
  // suffix in-tree tree. The roots of each tree have tree_state_id_ 0.
  class State {
   public:
    State() : state_id_(kNoStateId) {}

    State(const LexiconCompactor *c, StateId s)
        : c_(c), state_id_(s), is_in_prefix_out_tree_(false) {
      if (s >= c_->prefix_out_tree_.NumNodes()) {  // suffix in-tree
        is_in_prefix_out_tree_ = false;
        num_bridge_arcs_ = 0;
        tree_state_id_ = state_id_ - c_->prefix_out_tree_.NumNodes();
        if (tree_state_id_ == 0) {  // the final state has no outgoing arcs
          num_arcs_ = c_->closure_arcs_.size();
          num_tree_children_ = 0;
        } else {
          first_child_ = c->prefix_out_tree_.NumNodes() +
                         c->suffix_in_tree_.Parent(tree_state_id_);
          num_arcs_ = 1;
          num_tree_children_ = 1;
        }
      } else {  // prefix out-tree
        is_in_prefix_out_tree_ = true;
        if (state_id_ == 0) {  // root in the bridge arc bitmap is handled
                               // specially since selects0(-1) errors.
          num_bridge_arcs_ = c_->bridge_arc_bitmap_.Select0(state_id_);
          bridge_arc_index_ = 0;
        } else {
          const auto bridge_arc_range =
              c_->bridge_arc_bitmap_.Select0s(state_id_ - 1);
          num_bridge_arcs_ =
              bridge_arc_range.second - bridge_arc_range.first - 1;
          bridge_arc_index_ =
              c_->bridge_arc_bitmap_.Rank1(bridge_arc_range.first);
        }

        const auto first_and_last =
            c_->prefix_out_tree_.FirstAndLastChild(state_id_);
        first_child_ = first_and_last.first;
        if (first_and_last.first == std::numeric_limits<size_t>::max()) {
          // only bridge arcs (prefix out-tree leaf)
          num_tree_children_ = 0;
        } else {
          num_tree_children_ = first_and_last.second - first_and_last.first + 1;
        }
        num_arcs_ = num_tree_children_ + num_bridge_arcs_;
      }
    }

    StateId GetStateId() const { return state_id_; }

    Weight Final() const {
      return IsFinalState() ? Weight::One() : Weight::Zero();
    }

    Arc GetArc(size_t i, uint32_t flag = kArcValueFlags) const {
      if (is_in_prefix_out_tree_) {
        if (i < num_tree_children_) {  // Prefix out-tree children.
          const StateId next_state = first_child_ + i;
          return c_->element_compactor_.GetArc(
              c_->prefix_out_tree_reader_->Get(next_state - 1), next_state);
        } else {  // Now into the bridge arcs.
          const size_t element_location =
              bridge_arc_index_ + i - num_tree_children_;
          return c_->element_compactor_.GetBridgeArc(
              c_->ilabel_reader_->Get(element_location),
              c_->olabel_reader_->Get(element_location),
              c_->next_state_reader_->Get(element_location),
              c_->prefix_out_tree_.NumNodes());
        }
      } else {  // Suffix in-tree.
        if (tree_state_id_ == 0 && !c_->closure_arcs_.empty()) {
          return c_->closure_arcs_[i];
        } else {
          return c_->element_compactor_.GetReverseArc(
              c_->reverse_arcs_[tree_state_id_ - 1], first_child_);
        }
      }
    }

    size_t NumArcs() const { return num_arcs_; }

   private:
    inline bool IsFinalState() const {
      return !is_in_prefix_out_tree_ && (tree_state_id_ == 0);
    }

    const LexiconCompactor *c_;
    StateId state_id_;
    StateId tree_state_id_;
    StateId first_child_;
    size_t num_arcs_;
    size_t num_tree_children_;
    size_t bridge_arc_index_;
    size_t num_bridge_arcs_;
    bool is_in_prefix_out_tree_;
  };

  StateId Start() const { return NumStates() == 0 ? kNoStateId : 0; }

  StateId NumStates() const {
    return prefix_out_tree_.NumNodes() + suffix_in_tree_.NumNodes();
  }

  size_t NumArcs() const {
    return prefix_out_tree_.NumEdges() + suffix_in_tree_.NumEdges() +
           num_bridge_arcs_ + closure_arcs_.size();
  }

  bool IsCompatible(const Fst<Arc> &fst) const {
    return HasCompactLexiconStructure(fst) &&
           element_compactor_.IsCompatible(fst);
  }

  uint64_t Properties(uint64_t props) const {
    props &= kStateSortProperties;  // States are renumbered.
    // Arc order is not preserved. Bridge arcs are now seen after
    // prefix out-tree arcs. Prefix out-tree arcs have epsilon output
    // labels, and bridge arcs have non-epsilon input labels. Therefore,
    // ilabel order is not preserved in general. Olabel sorting is
    // preserved since prefix out-tree arcs have epsilon olabels, so
    // moving them first preserves the sorting (assuming olabels are
    // non-negative).
    // TODO: Either check for non-negativity of olabels
    // in IsCompatible, or clear the kOLabelSorted bit here if there
    // are negative olabels.
    props &= ~(kILabelSorted | kNotILabelSorted);
    auto cyclic_prop = closure_arcs_.empty() ? (kAcyclic | kInitialAcyclic)
                                             : (kCyclic | kInitialCyclic);
    return kExpanded | kAccessible | kCoAccessible | cyclic_prop | props;
  }

  static const std::string &Type() {
    std::string out = ElementCompactor::Type() + "lexicon-compactor";
    static const std::string *const type = new std::string(out);
    return *type;
  }

  bool Error() const { return false; }

  bool Write(std::ostream &strm, const FstWriteOptions &opts) const {
    prefix_out_tree_.Write(strm);
    suffix_in_tree_.Write(strm);
    // We write closure_arcs first so that we can find out the exact number of
    // each arc when are reading the data later.
    uint64_t closure_size = closure_arcs_.size();
    Write(strm, &closure_size, 1);
    Write(strm, &prefix_out_tree_shrunk_size_, 1);
    Write(strm, prefix_out_tree_array_.get(), prefix_out_tree_shrunk_size_);
    WriteType(strm, prefix_out_tree_decode_key_);
    Write(strm, reverse_arcs_.data(), reverse_arcs_.size());
    Write(strm, &ilabel_shrunk_size_, 1);
    Write(strm, ilabel_array_.get(), ilabel_shrunk_size_);
    WriteType(strm, ilabel_decode_key_);
    Write(strm, &olabel_shrunk_size_, 1);
    Write(strm, olabel_array_.get(), olabel_shrunk_size_);
    WriteType(strm, olabel_decode_key_);
    Write(strm, &next_state_shrunk_size_, 1);
    Write(strm, next_state_array_.get(), next_state_shrunk_size_);
    WriteType(strm, next_state_decode_key_);
    size_t bridge_arc_bitmapsize =
        num_bridge_arcs_ + prefix_out_tree_.NumNodes();
    Write(strm, bm_.get(), BitmapIndex::StorageSize(bridge_arc_bitmapsize));
    Write(strm, closure_arcs_.data(), closure_arcs_.size());
    return strm ? true : false;
  }

  static LexiconCompactor<Arc, SuffixILabel> *Read(std::istream &strm,
                                                   const FstReadOptions &opts,
                                                   const FstHeader &hdr) {
    size_t num_arcs = hdr.NumArcs();
    auto prefix_out_tree = LoudsTree::Read(strm);
    auto suffix_in_tree = LoudsTree::Read(strm);
    size_t num_prefix_out_tree_arcs = prefix_out_tree->NumEdges();
    size_t num_suffix_in_tree_arcs = suffix_in_tree->NumEdges();
    uint64_t num_closure_arcs;
    Read(strm, &num_closure_arcs, 1);
    const size_t num_bridge_arcs = num_arcs - num_prefix_out_tree_arcs -
                                   num_suffix_in_tree_arcs - num_closure_arcs;
    uint64_t prefix_out_tree_shrunk_size;
    uint64_t ilabel_shrunk_size;
    uint64_t olabel_shrunk_size;
    uint64_t next_state_shrunk_size;
    std::array<uint64_t, 2> prefix_out_tree_decode_key;
    std::array<uint64_t, 2> ilabel_decode_key;
    std::array<uint64_t, 2> olabel_decode_key;
    std::array<uint64_t, 2> next_state_decode_key;
    Read(strm, &prefix_out_tree_shrunk_size, 1);
    auto prefix_out_tree_array = make_unique_for_overwrite<uint64_t[]>(
        prefix_out_tree_shrunk_size);
    Read(strm, &prefix_out_tree_array[0], prefix_out_tree_shrunk_size);
    ReadType(strm, &prefix_out_tree_decode_key);
    std::vector<SuffixArcElement> reverse_arcs(num_suffix_in_tree_arcs);
    Read(strm, reverse_arcs.data(), reverse_arcs.size());
    Read(strm, &ilabel_shrunk_size, 1);
    auto ilabel_array =
        make_unique_for_overwrite<uint64_t[]>(ilabel_shrunk_size);
    Read(strm, &ilabel_array[0], ilabel_shrunk_size);
    ReadType(strm, &ilabel_decode_key);
    Read(strm, &olabel_shrunk_size, 1);
    auto olabel_array =
        make_unique_for_overwrite<uint64_t[]>(olabel_shrunk_size);
    Read(strm, &olabel_array[0], olabel_shrunk_size);
    ReadType(strm, &olabel_decode_key);
    Read(strm, &next_state_shrunk_size, 1);
    auto next_state_array =
        make_unique_for_overwrite<uint64_t[]>(next_state_shrunk_size);
    Read(strm, &next_state_array[0], next_state_shrunk_size);
    ReadType(strm, &next_state_decode_key);
    const size_t bridge_arc_bitmap_storage_size =
        BitmapIndex::StorageSize(prefix_out_tree->NumNodes() + num_bridge_arcs);
    auto bridge_arc_bitmap = make_unique_for_overwrite<uint64_t[]>(
        bridge_arc_bitmap_storage_size);
    Read(strm, bridge_arc_bitmap.get(), bridge_arc_bitmap_storage_size);
    std::vector<Arc> closure_arcs(num_closure_arcs);
    Read(strm, closure_arcs.data(), closure_arcs.size());
    if (!strm) return nullptr;
    return new LexiconCompactor(
        prefix_out_tree, suffix_in_tree, prefix_out_tree_shrunk_size,
        std::move(prefix_out_tree_array), prefix_out_tree_decode_key,
        reverse_arcs, ilabel_shrunk_size, std::move(ilabel_array),
        ilabel_decode_key, olabel_shrunk_size, std::move(olabel_array),
        olabel_decode_key, next_state_shrunk_size, std::move(next_state_array),
        next_state_decode_key, std::move(bridge_arc_bitmap), num_bridge_arcs,
        std::move(closure_arcs));
  }

  void SetState(StateId s, State *state) const { *state = State(this, s); }

 private:
  enum {
    kShrunkArrayComplexity = 3,
  };

  template <typename T>
  static std::unique_ptr<T[]> CopyArray(const T *v, const size_t n) {
    auto a = make_unique_for_overwrite<T[]>(n);
    std::copy_n(v, n, a.get());
    return a;
  }

  LexiconCompactor(LoudsTree &&prefix_out_tree, LoudsTree &&suffix_in_tree,
                   uint64_t prefix_out_tree_shrunk_size,
                   std::unique_ptr<const uint64_t[]> prefix_out_tree_array,
                   const std::array<uint64_t, 2> &prefix_out_tree_decode_key,
                   std::vector<SuffixArcElement> reverse_arcs,
                   uint64_t ilabel_shrunk_size,
                   std::unique_ptr<const uint64_t[]> ilabel_array,
                   const std::array<uint64_t, 2> &ilabel_decode_key,
                   uint64_t olabel_shrunk_size,
                   std::unique_ptr<const uint64_t[]> olabel_array,
                   const std::array<uint64_t, 2> &olabel_decode_key,
                   int64_t next_state_shrunk_size,
                   std::unique_ptr<const uint64_t[]> next_state_array,
                   const std::array<uint64_t, 2> &next_state_decode_key,
                   std::unique_ptr<uint64_t[]> bm, uint64_t num_bridge_arcs,
                   std::vector<Arc> &&closure_arcs)
      : prefix_out_tree_(std::move(prefix_out_tree)),
        suffix_in_tree_(std::move(suffix_in_tree)),
        reverse_arcs_(reverse_arcs),
        prefix_out_tree_shrunk_size_(prefix_out_tree_shrunk_size),
        ilabel_shrunk_size_(ilabel_shrunk_size),
        olabel_shrunk_size_(olabel_shrunk_size),
        next_state_shrunk_size_(next_state_shrunk_size),
        prefix_out_tree_array_(std::move(prefix_out_tree_array)),
        ilabel_array_(std::move(ilabel_array)),
        olabel_array_(std::move(olabel_array)),
        next_state_array_(std::move(next_state_array)),
        prefix_out_tree_decode_key_(prefix_out_tree_decode_key),
        ilabel_decode_key_(ilabel_decode_key),
        olabel_decode_key_(olabel_decode_key),
        next_state_decode_key_(next_state_decode_key),
        num_bridge_arcs_(num_bridge_arcs),
        bm_(std::move(bm)),
        closure_arcs_(std::move(closure_arcs)),
        element_compactor_() {
    bridge_arc_bitmap_.BuildIndex(
        bm_.get(), prefix_out_tree_.NumNodes() + num_bridge_arcs);
    prefix_out_tree_reader_.reset(ShrunkArray::Reader::New());
    prefix_out_tree_reader_->Bind(prefix_out_tree_array_.get(),
                                  prefix_out_tree_decode_key_.data());
    ilabel_reader_.reset(ShrunkArray::Reader::New());
    ilabel_reader_->Bind(ilabel_array_.get(), ilabel_decode_key_.data());
    olabel_reader_.reset(ShrunkArray::Reader::New());
    olabel_reader_->Bind(olabel_array_.get(), olabel_decode_key_.data());
    next_state_reader_.reset(ShrunkArray::Reader::New());
    next_state_reader_->Bind(next_state_array_.get(),
                             next_state_decode_key_.data());
  }

  template <typename T>
  void Write(std::ostream &strm, const T *t, size_t length) const {
    strm.write(reinterpret_cast<const char *>(t), length * sizeof(T));
  }

  template <typename T>
  static void Read(std::istream &strm, T *t, size_t length) {
    strm.read(reinterpret_cast<char *>(t), length * sizeof(T));
  }

  LoudsTree prefix_out_tree_;
  LoudsTree suffix_in_tree_;
  std::vector<SuffixArcElement> reverse_arcs_;
  uint64_t prefix_out_tree_shrunk_size_ = 0;
  uint64_t ilabel_shrunk_size_ = 0;
  uint64_t olabel_shrunk_size_ = 0;
  uint64_t next_state_shrunk_size_ = 0;
  std::unique_ptr<const uint64_t[]> prefix_out_tree_array_;
  std::unique_ptr<const uint64_t[]> ilabel_array_;
  std::unique_ptr<const uint64_t[]> olabel_array_;
  std::unique_ptr<const uint64_t[]> next_state_array_;
  std::unique_ptr<ShrunkArray::Reader> prefix_out_tree_reader_;
  std::unique_ptr<ShrunkArray::Reader> ilabel_reader_;
  std::unique_ptr<ShrunkArray::Reader> olabel_reader_;
  std::unique_ptr<ShrunkArray::Reader> next_state_reader_;
  std::array<uint64_t, 2> prefix_out_tree_decode_key_{0, 0};
  std::array<uint64_t, 2> ilabel_decode_key_{0, 0};
  std::array<uint64_t, 2> olabel_decode_key_{0, 0};
  std::array<uint64_t, 2> next_state_decode_key_{0, 0};
  size_t num_bridge_arcs_ = 0;
  std::unique_ptr<uint64_t[]> bm_;
  BitmapIndex bridge_arc_bitmap_;
  std::vector<Arc> closure_arcs_;
  ElementCompactor element_compactor_;
};

template <class Arc, class SuffixILabel>
using CompactLexiconFst = CompactFst<Arc, LexiconCompactor<Arc, SuffixILabel>,
                                     DefaultCacheStore<Arc>>;

// Checks for the required structure of a lexicon. We care that it is
// deterministic, acyclic (other than closure arcs), and trim.
// There should be no epsilon input or output paths. Removing the bridge
// arcs and closure arcs should create exactly two connected components,
// both of which are trees.
template <typename Arc>
bool HasCompactLexiconStructure(const Fst<Arc> &fst) {
  using StateId = typename Fst<Arc>::StateId;
  using Weight = typename Fst<Arc>::Weight;
  if (fst.Start() == kNoStateId) return true;
  size_t num_states = 0;
  size_t num_arcs = 0;
  size_t num_tree_arcs = 0;
  size_t num_bridge_arcs = 0;
  size_t num_closure_arcs = 0;
  StateId final_state = kNoStateId;
  for (StateIterator<Fst<Arc>> siter(fst); !siter.Done(); siter.Next()) {
    const StateId state = siter.Value();
    ++num_states;
    num_arcs += fst.NumArcs(state);
    if (fst.Final(state) != Weight::Zero()) {
      if (fst.Final(state) != Weight::One()) {
        VLOG(2) << __func__ << ": Final state with weight != 1";
        return false;
      }
      if (final_state != kNoStateId) {
        VLOG(2) << __func__ << ": Multiple final states: " << final_state
                << " and " << state;
        return false;
      }
      final_state = state;
    } else {
      for (ArcIterator<Fst<Arc>> aiter(fst, siter.Value()); !aiter.Done();
           aiter.Next()) {
        const auto arc = aiter.Value();
        if (arc.weight != Weight::One()) {
          VLOG(2) << __func__ << ": Arc with weight != 1";
          return false;
        }
        if (arc.olabel != 0) {  // Bridge arc.
          ++num_bridge_arcs;
        } else {
          ++num_tree_arcs;
        }
      }
    }
  }
  if (final_state == kNoStateId) {
    VLOG(2) << __func__ << ": No final state found";
    return false;
  }
  if (fst.Final(final_state) != Weight::One()) {
    VLOG(2) << __func__ << ": Final state with weight != 1";
    return false;
  }
  for (ArcIterator<Fst<Arc>> aiter(fst, final_state); !aiter.Done();
       aiter.Next()) {
    const auto arc = aiter.Value();
    if (arc.nextstate != fst.Start()) {
      VLOG(2) << __func__ << ": Final state with arc to " << arc.nextstate
              << " instead of start state: " << fst.Start();
      return false;
    }
    ++num_closure_arcs;
  }
  const uint64_t props = (num_closure_arcs == 0 ? kAcyclic : kCyclic) |
                         kAccessible | kCoAccessible;
  if (fst.Properties(props, true) != props) {
    VLOG(2) << __func__ << ": Bad props: " << std::hex
            << fst.Properties(props, true);
    return false;
  }
  const size_t expected_num_arcs =
      num_states - 2 + num_bridge_arcs + num_closure_arcs;
  if (num_arcs != expected_num_arcs) {
    VLOG(2) << __func__ << ": Bad num_arcs: " << num_arcs
            << " expected_num_arcs: " << expected_num_arcs
            << " num_states: " << num_states
            << " num_bridge_arcs: " << num_bridge_arcs
            << " num_closure_arcs: " << num_closure_arcs;
    return false;
  }
  if (num_arcs != num_bridge_arcs + num_tree_arcs + num_closure_arcs) {
    VLOG(2) << __func__ << ": Bad num_arcs: " << num_arcs
            << " num_bridge_arcs: " << num_bridge_arcs
            << " num_tree_arcs: " << num_tree_arcs
            << " num_closure_arcs: " << num_closure_arcs;
    return false;
  }

  // Checks the tree/bridge arc topology. We only need to check accessible
  // states (by non-bridge arcs) and then use some set operations since we
  // know all states are accessible (by any arc) from the above check.
  absl::flat_hash_set<StateId> accessible;
  absl::flat_hash_set<StateId> bridge_arc_accessible;
  std::queue<StateId> q;
  q.push(fst.Start());
  while (!q.empty()) {
    const auto cur = q.front();
    q.pop();
    accessible.insert(cur);
    for (ArcIterator<Fst<Arc>> aiter(fst, cur); !aiter.Done(); aiter.Next()) {
      const auto arc = aiter.Value();
      if (arc.olabel == 0) {
        q.push(arc.nextstate);
      } else {
        bridge_arc_accessible.insert(arc.nextstate);
      }
    }
  }
  if (accessible.count(final_state)) {
    VLOG(2) << __func__
            << ": Final state must not be accessible by non-bridge-arcs";
    return false;
  }
  std::vector<StateId> intersection;
  std::set_intersection(
      accessible.begin(), accessible.end(), bridge_arc_accessible.begin(),
      bridge_arc_accessible.end(), std::back_inserter(intersection));
  if (!intersection.empty()) {
    VLOG(2) << __func__
            << ": Intersection of states accessible by bridge-arcs and "
            << "arcs must be empty, had size " << intersection.size()
            << ", first: " << intersection.front();
    return false;
  }
  return true;
}

}  // namespace fst

#endif  // OPENFST_EXTENSIONS_NGRAM_LOUDS_LEXICON_H_
