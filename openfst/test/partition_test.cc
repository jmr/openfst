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
// Unit test for the Partition class.

#include "openfst/lib/partition.h"

#include <memory>

#include "gtest/gtest.h"
#include "absl/log/log.h"
#include "openfst/lib/queue.h"

namespace fst {
namespace {

using ::fst::internal::Partition;
using ::fst::internal::PartitionIterator;

class PartitionTest : public testing::Test {
 protected:
  void SetUp() override {
    partition_ = std::make_unique<Partition<int>>(1000);
    // Places all states into an initial partition.
    int class_id = partition_->AddClass();
    for (int i = 0; i < 1000; ++i) partition_->Add(i, class_id);
  }

  std::unique_ptr<Partition<int>> partition_;
};

using Queue = FifoQueue<int>;

TEST_F(PartitionTest, FinalNonFinal) {
  // states 990 -> 1000 are finals
  int new_class = partition_->AddClass();
  for (int i = 990; i < 1000; ++i) partition_->Move(i, new_class);

  // validate split (larger class == 0)
  for (int i = 0; i < 990; ++i) ASSERT_EQ(partition_->ClassId(i), 0);
  // validate split (smaller class == 1)
  for (int i = 990; i < 1000; ++i) ASSERT_EQ(partition_->ClassId(i), 1);

  // iterator over each class
  for (PartitionIterator<int> siter(*partition_, 0); !siter.Done();
       siter.Next()) {
    ASSERT_GE(siter.Value(), 0);
    ASSERT_LT(siter.Value(), 990);
  }
  for (PartitionIterator<int> siter(*partition_, 1); !siter.Done();
       siter.Next()) {
    ASSERT_GE(siter.Value(), 990);
    ASSERT_LT(siter.Value(), 1000);
  }
}

TEST_F(PartitionTest, ThreeWaySplitUsingMove) {
  // states 990 -> 1000 are finals
  Queue queue;
  queue.Enqueue(0);

  int new_class1 = partition_->AddClass();
  for (int i = 990; i < 1000; ++i) partition_->Move(i, new_class1);
  queue.Enqueue(new_class1);

  int new_class2 = partition_->AddClass();
  for (int i = 100; i < 200; ++i) partition_->Move(i, new_class2);
  queue.Enqueue(new_class2);

  // validate split
  //  Largest class = 0
  for (int i = 0; i < 100; ++i) ASSERT_EQ(partition_->ClassId(i), 0);
  for (int i = 200; i < 990; ++i) ASSERT_EQ(partition_->ClassId(i), 0);

  // first smaller = 1
  for (int i = 990; i < 1000; ++i) ASSERT_EQ(partition_->ClassId(i), 1);

  // second smaller = 2
  for (int i = 100; i < 200; ++i) ASSERT_EQ(partition_->ClassId(i), 2);

  LOG(INFO) << queue.Head() << " " << partition_->ClassSize(queue.Head());
  queue.Dequeue();
  LOG(INFO) << queue.Head() << " " << partition_->ClassSize(queue.Head());
  queue.Dequeue();
  LOG(INFO) << queue.Head() << " " << partition_->ClassSize(queue.Head());
}

TEST_F(PartitionTest, ThreeWaySplitUsingSplit) {
  // states 990 -> 1000 are finals
  Queue queue;
  queue.Enqueue(0);

  // split 1
  for (int i = 990; i < 1000; ++i) partition_->SplitOn(i);
  partition_->FinalizeSplit(&queue);

  // split 2
  for (int i = 100; i < 200; ++i) partition_->SplitOn(i);
  partition_->FinalizeSplit(&queue);

  // validate split
  //  Largest class = 0
  for (int i = 0; i < 100; ++i) ASSERT_EQ(partition_->ClassId(i), 0);
  for (int i = 200; i < 990; ++i) ASSERT_EQ(partition_->ClassId(i), 0);

  // first smaller = 1
  for (int i = 990; i < 1000; ++i) ASSERT_EQ(partition_->ClassId(i), 1);

  // second smaller = 2
  for (int i = 100; i < 200; ++i) ASSERT_EQ(partition_->ClassId(i), 2);

  LOG(INFO) << queue.Head() << " " << partition_->ClassSize(queue.Head());
  queue.Dequeue();
  LOG(INFO) << queue.Head() << " " << partition_->ClassSize(queue.Head());
  queue.Dequeue();
  LOG(INFO) << queue.Head() << " " << partition_->ClassSize(queue.Head());
}

}  // namespace
}  // namespace fst
