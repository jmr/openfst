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
// Unit test for FST caching.

#include "openfst/lib/cache.h"

#include "gtest/gtest.h"
#include "openfst/lib/arc.h"

namespace fst {
namespace {

using Arc = StdArc;
using Label = Arc::Label;
using StateId = Arc::StateId;
using Weight = Arc::Weight;

using State = CacheState<Arc>;

constexpr int kMinCacheSize = 8096;

class CacheTest : public testing::Test {
 protected:
  // Tests when garbage collection is not enabled.
  template <class CacheStore>
  void NoGCTest() {
    CacheOptions opts(false, kMinCacheSize);
    CacheStore store(opts);
    ASSERT_TRUE(store.GetState(2) == nullptr);
    const auto *state2 = store.GetMutableState(2);
    ASSERT_TRUE(state2 != nullptr);
    ASSERT_TRUE(store.GetState(4) == nullptr);
    auto *state4 = store.GetMutableState(4);
    ASSERT_TRUE(state4 != nullptr);

    // Copies.
    {
      CacheStore copy_store(store);
      ASSERT_TRUE(copy_store.GetState(2) != nullptr);
      ASSERT_TRUE(copy_store.GetState(4) != nullptr);
    }

    // Adds arcs.
    Arc arc(0, 0, Weight(0.0), 0);
    store.AddArc(state4, arc);
    store.AddArc(state4, arc);
    ASSERT_EQ(state4->NumArcs(), 2);
    ASSERT_EQ(state4->NumInputEpsilons(), 2);
    ASSERT_EQ(state4->NumOutputEpsilons(), 2);

    // Deletes arcs.
    store.DeleteArcs(state4, 1);
    ASSERT_EQ(state4->NumArcs(), 1);
    ASSERT_EQ(state4->NumInputEpsilons(), 1);
    ASSERT_EQ(state4->NumOutputEpsilons(), 1);

    store.DeleteArcs(state4);
    ASSERT_EQ(state4->NumArcs(), 0);
    ASSERT_EQ(state4->NumInputEpsilons(), 0);
    ASSERT_EQ(state4->NumOutputEpsilons(), 0);

    // Pushs/sets arcs.
    state4->PushArc(arc);
    state4->PushArc(arc);
    store.SetArcs(state4);
    ASSERT_EQ(state4->NumArcs(), 2);
    ASSERT_EQ(state4->NumInputEpsilons(), 2);
    ASSERT_EQ(state4->NumOutputEpsilons(), 2);

    // Deletes all states.
    store.Clear();
    ASSERT_TRUE(store.GetState(4) == nullptr);
  }

  // Tests when first-state garbage collection is enabled.
  template <class CacheStore>
  void FirstTest() {
    CacheOptions opts(true, 0);
    CacheStore store(opts);

    ASSERT_TRUE(store.GetState(0) == nullptr);
    const auto *state0 = store.GetMutableState(0);
    ASSERT_TRUE(state0 != nullptr);

    ASSERT_TRUE(store.GetState(2) == nullptr);
    const auto *state2 = store.GetMutableState(2);
    ASSERT_TRUE(state2 != nullptr);

    // Checks that state 0 has been GC'ed.
    ASSERT_TRUE(store.GetState(0) == nullptr);

    ASSERT_TRUE(store.GetState(4) == nullptr);
    const auto *state4 = store.GetMutableState(4);
    ASSERT_TRUE(state4 != nullptr);

    // Checks that state 2 has been GC'ed.
    int gc_state = -1;
    if (store.GetState(0) == nullptr) gc_state = 0;
    if (store.GetState(2) == nullptr) gc_state = 2;
    ASSERT_NE(gc_state, -1);

    state4->IncrRefCount();
    ASSERT_TRUE(store.GetState(6) == nullptr);
    State *state6 = store.GetMutableState(6);
    ASSERT_TRUE(state6 != nullptr);

    // Checks that state 4 has not been GC'ed.
    ASSERT_TRUE(store.GetState(4) != nullptr);

    // Deletes state 4.
    store.Reset();
    while (!store.Done()) {
      StateId s = store.Value();
      if (s == 4) {
        store.Delete();
      } else {
        store.Next();
      }
    }
    ASSERT_TRUE(store.GetState(4) == nullptr);
  }

  // Tests when standard garbage collection is enabled.
  template <class CacheStore>
  void GCTest() {
    CacheOptions opts(true, kMinCacheSize);
    CacheStore store(opts);

    ASSERT_TRUE(store.GetState(0) == nullptr);
    auto *state0 = store.GetMutableState(0);
    ASSERT_TRUE(state0 != nullptr);
    state0->IncrRefCount();

    ASSERT_TRUE(store.GetState(2) == nullptr);
    const auto *state2 = store.GetMutableState(2);
    ASSERT_TRUE(state2 != nullptr);

    ASSERT_TRUE(store.GetState(4) == nullptr);
    auto *state4 = store.GetMutableState(4);
    ASSERT_TRUE(state4 != nullptr);

    // Checks that states 0 & 2 have not been GC'ed.
    ASSERT_TRUE(store.GetState(0) != nullptr);
    ASSERT_TRUE(store.GetState(2) != nullptr);

    Arc arc(0, 0, Weight(0.0), 0);
    for (auto i = 0; i < kMinCacheSize / sizeof(Arc); ++i) state4->PushArc(arc);
    store.SetArcs(state4);

    // Checks that state 0 has not been GC'ed and that state 2 has been GC'ed.
    int gc_state = -1;
    if (store.GetState(0) == nullptr) gc_state = 0;
    if (store.GetState(2) == nullptr) gc_state = 2;
    ASSERT_NE(gc_state, -1);

    state0->DecrRefCount();

    auto *state6 = store.GetMutableState(6);
    ASSERT_TRUE(state6 != nullptr);

    for (auto i = 0; i < kMinCacheSize / sizeof(Arc); ++i) state6->PushArc(arc);
    store.SetArcs(state6);

    // Checks that state 0 and 4 have been GC'ed and
    // checks that state 6 has not been GC'ed.
    ASSERT_TRUE(store.GetState(gc_state) == nullptr);
    ASSERT_TRUE(store.GetState(4) == nullptr);
    ASSERT_TRUE(store.GetState(6) != nullptr);
  }
};

TEST_F(CacheTest, VectorCacheStoreTest) { NoGCTest<VectorCacheStore<State>>(); }

TEST_F(CacheTest, HashCacheStoreTest) { NoGCTest<HashCacheStore<State>>(); }

TEST_F(CacheTest, FirstVectorCacheStoreTest) {
  using CacheStore = FirstCacheStore<VectorCacheStore<State>>;
  NoGCTest<CacheStore>();
  FirstTest<CacheStore>();
}

TEST_F(CacheTest, GCVectorCacheStoreTest) {
  using CacheStore = GCCacheStore<VectorCacheStore<State>>;
  NoGCTest<CacheStore>();
  GCTest<CacheStore>();
}

TEST_F(CacheTest, GCHashCacheStoreTest) {
  using CacheStore = GCCacheStore<HashCacheStore<State>>;
  NoGCTest<CacheStore>();
  GCTest<CacheStore>();
}

TEST_F(CacheTest, DefaultCacheStoreTest) {
  using CacheStore = DefaultCacheStore<Arc>;
  NoGCTest<CacheStore>();
  FirstTest<CacheStore>();
  GCTest<CacheStore>();
}

}  // namespace
}  // namespace fst
