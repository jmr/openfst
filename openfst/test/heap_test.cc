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

#include "openfst/lib/heap.h"

#include <ostream>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "absl/log/log.h"
#include "absl/random/random.h"

namespace fst {
namespace {

class HeapTest : public testing::Test {
 protected:
  using IntPair = std::pair<int, int>;

  struct IntPairCompare {
    bool operator()(const IntPair &a, const IntPair &b) const {
      return a.first > b.first;
    }
  };

  using IntPairHeap = Heap<IntPair, IntPairCompare>;
  IntPairHeap heap_;

  const std::vector<int> values_ = {4, 8, 3, 5, 1, 7, 9};
};

TEST_F(HeapTest, Empty) {
  EXPECT_TRUE(heap_.Empty());
  heap_.Insert({0, 0});
  EXPECT_FALSE(heap_.Empty());
}

TEST_F(HeapTest, Size) {
  EXPECT_EQ(0, heap_.Size());
  for (int i = 0; i < 10; ++i) {
    heap_.Insert({i, i});
    EXPECT_EQ(i + 1, heap_.Size());
  }
}

TEST_F(HeapTest, Clear) {
  heap_.Insert({0, 0});
  heap_.Insert({1, 1});
  heap_.Clear();
  EXPECT_TRUE(heap_.Empty());
}

TEST_F(HeapTest, Insert) {
  for (int i = 10; i <= 20; ++i) {
    heap_.Insert({i, i});
    EXPECT_EQ(IntPair(i, i), heap_.Top());
  }
  EXPECT_EQ(IntPair(20, 20), heap_.Top());
  heap_.Insert({1, 1});  // Insert at bottom.
  EXPECT_EQ(IntPair(20, 20), heap_.Top());
  heap_.Insert({21, 21});  // Insert at front
  EXPECT_EQ(IntPair(21, 21), heap_.Top());
}

TEST_F(HeapTest, Pop) {
  for (int i = 0; i < values_.size(); ++i) {
    heap_.Insert({values_[i], i});
  }
  int prev = 10;
  for (int i = values_.size(); i > 0; --i) {
    EXPECT_EQ(i, heap_.Size());
    const auto v = heap_.Pop();
    EXPECT_GT(prev, v.first);
  }
  EXPECT_TRUE(heap_.Empty());
}

TEST_F(HeapTest, Key) {
  std::vector<int> keys;
  for (int i = 0; i < values_.size(); ++i) {
    keys.push_back(heap_.Insert({values_[i], i}));
    EXPECT_GE(keys[i], 0);
  }
  for (int i = 0; i < values_.size(); ++i) {
    EXPECT_EQ(i, heap_.Get(i).second);
    EXPECT_EQ(values_[i], heap_.Get(i).first);
  }
  // Removes three elements.
  EXPECT_EQ(9, heap_.Pop().first);
  EXPECT_EQ(8, heap_.Pop().first);
  EXPECT_EQ(7, heap_.Pop().first);
  // Checks that keys are still valid.
  for (int i = 0; i < values_.size(); ++i) {
    if (values_[i] < 7) {
      EXPECT_EQ(i, heap_.Get(i).second);
      EXPECT_EQ(values_[i], heap_.Get(i).first);
    }
  }
  // Adds two more elements.
  heap_.Insert({0, 100});
  heap_.Insert({20, 100});
  // Checks that keys are still valid.
  for (int i = 0; i < values_.size(); ++i) {
    if (values_[i] < 7) {
      EXPECT_EQ(i, heap_.Get(i).second);
      EXPECT_EQ(values_[i], heap_.Get(i).first);
    }
  }
}

TEST_F(HeapTest, UpdateSameOrder) {
  std::vector<int> keys;
  for (int i = 0; i < values_.size(); ++i) {
    keys.push_back(heap_.Insert({values_[i], i}));
  }
  // Updates additional value, same compared value.
  for (int i = 0; i < values_.size(); ++i) {
    const int key = keys[i];
    EXPECT_EQ(IntPair(values_[i], i), heap_.Get(key));
    heap_.Update(key, {values_[i], i + 100});
  }
  for (int i = 0; i < values_.size(); ++i) {
    EXPECT_EQ(IntPair(values_[i], i + 100), heap_.Get(keys[i]));
  }
  EXPECT_EQ(IntPair(9, 106), heap_.Top());
}

TEST_F(HeapTest, UpdateReverseOrder) {
  std::vector<int> keys;
  for (int i = 0; i < values_.size(); ++i) {
    keys.push_back(heap_.Insert({values_[i], i}));
  }
  for (int i = 0; i < values_.size(); ++i) {
    const auto key = keys[i];
    EXPECT_EQ(IntPair(values_[i], i), heap_.Get(key));
    heap_.Update(key, {-values_[i], i});
  }
  for (int i = 0; i < values_.size(); ++i) {
    EXPECT_EQ(IntPair(-values_[i], i), heap_.Get(keys[i]));
  }
  EXPECT_EQ(IntPair(-1, 4), heap_.Top());
}

TEST_F(HeapTest, UpdateOrder) {
  std::vector<int> keys;
  for (int i = 0; i < values_.size(); ++i) {
    keys.push_back(heap_.Insert({values_[i], i}));
  }
  for (int i = 0; i < values_.size(); ++i) {
    const auto key = keys[i];
    EXPECT_EQ(IntPair(values_[i], i), heap_.Get(key));
    heap_.Update(key, {2 * values_[i], i});
  }
  for (int i = 0; i < values_.size(); ++i) {
    EXPECT_EQ(IntPair(2 * values_[i], i), heap_.Get(keys[i]));
  }
  EXPECT_EQ(IntPair(18, 6), heap_.Top());
}

// A binary function for determining less than properties of pointers to data
// structs. Comparison is made on data struct by dereferenceing.
template <class T>
struct lesser_ptr {
  bool operator()(const T &x, const T &y) const { return *x < *y; }
};

// A binary function for determining greater than properties of pointers to data
// structs. Comparison is made on data struct by dereferencing.
template <class T>
struct greater_ptr {
  using first_argument_type = T;
  using second_argument_type = T;
  using result_type = bool;
  bool operator()(const T &x, const T &y) const { return *y < *x; }
};

// A small chunk of memory for the object unit test.
struct Thingy {
  bool operator<(const Thingy &a) const { return (val < a.val); }

  int val;
  int key;
};

// Test fixture for heap unit test. H denotes the heap type.
template <class H>
class ObjectHeapTest : public testing::Test {
 protected:
  ObjectHeapTest() {}

  H heap_;
  std::vector<Thingy *> obj_vec_{500};
  absl::BitGen random_;

  void SetUp() override {
    VLOG(1) << "-->Adding a bunch of random Thingys<--";
    for (int i = 0; i < obj_vec_.size(); i++) {
      obj_vec_[i] = new Thingy();
      obj_vec_[i]->val = absl::Uniform(random_, 0, 1000);
      obj_vec_[i]->key = heap_.Insert(obj_vec_[i]);
    }
  }

  void TearDown() override {
    VLOG(1) << "-->Checking vector for all keys popped<--" << std::endl;
    for (int i = 0; i < obj_vec_.size(); ++i) {
      ASSERT_EQ(static_cast<Thingy *>(nullptr), obj_vec_[i]);
    }
  }

  void PopTest1() {
    VLOG(1) << "-->Popping some Thingys<--" << std::endl;
    int max = 0;
    for (int i = 0; i < 10; i++) {
      const auto *obj = heap_.Pop();
      VLOG(1) << "pop: " << obj->key << " " << obj->val << std::endl;
      obj_vec_[obj->key] = nullptr;
      ASSERT_TRUE(obj->val >= max);
      max = obj->val;
      delete obj;
    }
  }

  void InsertTest1() {
    VLOG(1) << "-->Inserting some back<--" << std::endl;
    for (int i = 0; i < 5; i++) {
      auto *obj = new Thingy();
      obj->val = absl::Uniform(random_, 0, 1000);
      obj->key = heap_.Insert(obj);
      VLOG(1) << "insert: " << obj->key << " " << obj->val << std::endl;
      ASSERT_EQ(static_cast<Thingy *>(nullptr), obj_vec_[obj->key]);
      obj_vec_[obj->key] = obj;
    }
  }

  void UpdateTest() {
    VLOG(1) << "-->Updating some in place<--" << std::endl;
    for (int i = 0; i < 100; ++i) {
      const int j = absl::Uniform(random_, 0,
                                  static_cast<int>(obj_vec_.size()));
      if (obj_vec_[j] != nullptr) {
        obj_vec_[j]->val = absl::Uniform(random_, 0, 1000);
        heap_.Update(obj_vec_[j]->key, obj_vec_[j]);
      }
    }
  }

  void InsertTest2() {
    VLOG(1) << "-->Inserting some more back<--" << std::endl;
    for (int i = 0; i < 5; i++) {
      auto *obj = new Thingy();
      obj->val = absl::Uniform(random_, 0, 1000);
      obj->key = heap_.Insert(obj);
      VLOG(1) << "insert: " << obj->key << " " << obj->val << std::endl;
      ASSERT_TRUE(obj_vec_[obj->key] == nullptr);
      obj_vec_[obj->key] = obj;
    }
  }

  void PopTest2() {
    VLOG(1) << "-->Popping all of them off<--" << std::endl;
    int max = 0;
    int num_items = 0;
    while (!heap_.Empty()) {
      const auto *obj = heap_.Pop();
      ASSERT_TRUE(obj->val >= max);
      ASSERT_TRUE(obj_vec_[obj->key] != nullptr);
      max = obj->val;
      num_items++;
      obj_vec_[obj->key] = nullptr;
      delete obj;
    }
  }
};

using MinHeap = Heap<Thingy *, lesser_ptr<Thingy *>>;
using ObjectHeapTestTypes = testing::Types<MinHeap>;

TYPED_TEST_SUITE(ObjectHeapTest, ObjectHeapTestTypes);

TYPED_TEST(ObjectHeapTest, HeapOperations) {
  this->PopTest1();
  this->InsertTest1();
  this->UpdateTest();
  this->InsertTest2();
  this->PopTest2();
}

}  // namespace
}  // namespace fst
