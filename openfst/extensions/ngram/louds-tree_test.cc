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

#include "openfst/extensions/ngram/louds-tree.h"

#include <cstddef>
#include <limits>
#include <vector>

#include "gtest/gtest.h"

namespace fst {

TEST(LoudsTreeTest, CompleteBinaryTreeTest) {
  // complete binary tree with 3 levels
  std::vector<size_t> tree{2, 2, 2, 0, 0, 0, 0};

  LoudsTree lt;
  ASSERT_TRUE(lt.Init(tree));
  ASSERT_TRUE(lt.IsInitialized());
  EXPECT_EQ(lt.NumNodes(), 7);
  EXPECT_EQ(lt.NumEdges(), 6);

  EXPECT_FALSE(lt.IsLeaf(0));
  EXPECT_FALSE(lt.IsLeaf(1));
  EXPECT_FALSE(lt.IsLeaf(2));
  EXPECT_TRUE(lt.IsLeaf(3));
  EXPECT_TRUE(lt.IsLeaf(4));
  EXPECT_TRUE(lt.IsLeaf(5));
  EXPECT_TRUE(lt.IsLeaf(6));
  EXPECT_EQ(1, lt.FirstChild(0));
  EXPECT_EQ(3, lt.FirstChild(1));
  EXPECT_EQ(5, lt.FirstChild(2));

  EXPECT_EQ(2, lt.LastChild(0));
  EXPECT_EQ(4, lt.LastChild(1));
  EXPECT_EQ(6, lt.LastChild(2));

  EXPECT_EQ(std::numeric_limits<size_t>::max(), lt.FirstChild(3));
  EXPECT_EQ(std::numeric_limits<size_t>::max(), lt.LastChild(3));
  EXPECT_EQ(std::numeric_limits<size_t>::max(), lt.FirstChild(4));
  EXPECT_EQ(std::numeric_limits<size_t>::max(), lt.LastChild(4));
  EXPECT_EQ(std::numeric_limits<size_t>::max(), lt.FirstChild(5));
  EXPECT_EQ(std::numeric_limits<size_t>::max(), lt.LastChild(5));
  EXPECT_EQ(std::numeric_limits<size_t>::max(), lt.FirstChild(6));
  EXPECT_EQ(std::numeric_limits<size_t>::max(), lt.LastChild(6));

  EXPECT_EQ(2, lt.LastChild(0));
  EXPECT_EQ(2, lt.NumChildren(0));
  EXPECT_EQ(2, lt.NumChildren(1));
  EXPECT_EQ(2, lt.NumChildren(2));
  EXPECT_EQ(0, lt.NumChildren(3));

  EXPECT_EQ(std::numeric_limits<size_t>::max(), lt.Parent(0));
  EXPECT_EQ(0, lt.Parent(1));
  EXPECT_EQ(0, lt.Parent(2));
  EXPECT_EQ(1, lt.Parent(3));
  EXPECT_EQ(1, lt.Parent(4));
  EXPECT_EQ(2, lt.Parent(5));
  EXPECT_EQ(2, lt.Parent(6));
}

TEST(LoudsTreeTest, WideTreeTest) {
  std::vector<size_t> tree{4, 4, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0};

  LoudsTree lt;
  ASSERT_TRUE(lt.Init(tree));
  ASSERT_TRUE(lt.IsInitialized());
  EXPECT_EQ(lt.NumNodes(), 13);
  EXPECT_EQ(lt.NumEdges(), 12);

  EXPECT_FALSE(lt.IsLeaf(0));
  EXPECT_FALSE(lt.IsLeaf(1));
  EXPECT_TRUE(lt.IsLeaf(2));
  EXPECT_TRUE(lt.IsLeaf(3));
  EXPECT_FALSE(lt.IsLeaf(4));
  EXPECT_TRUE(lt.IsLeaf(5));
  EXPECT_TRUE(lt.IsLeaf(6));
  EXPECT_TRUE(lt.IsLeaf(7));
  EXPECT_TRUE(lt.IsLeaf(8));
  EXPECT_TRUE(lt.IsLeaf(9));
  EXPECT_TRUE(lt.IsLeaf(10));
  EXPECT_TRUE(lt.IsLeaf(11));
  EXPECT_TRUE(lt.IsLeaf(12));
  EXPECT_TRUE(lt.IsLeaf(13));
  EXPECT_TRUE(lt.IsLeaf(14));

  EXPECT_EQ(1, lt.NthChild(0, 1));
  EXPECT_EQ(2, lt.NthChild(0, 2));
  EXPECT_EQ(3, lt.NthChild(0, 3));
  EXPECT_EQ(4, lt.NthChild(0, 4));
  EXPECT_EQ(std::numeric_limits<size_t>::max(), lt.NthChild(0, 5));

  EXPECT_TRUE(lt.IsChild(0, 1));
  EXPECT_FALSE(lt.IsChild(1, 0));
  EXPECT_FALSE(lt.IsChild(0, 8));
  EXPECT_FALSE(lt.IsChild(7, 9));
}

TEST(LoudsTreeTest, EmptyTreeTest) {
  std::vector<size_t> tree;

  LoudsTree lt;
  ASSERT_TRUE(lt.Init(tree));
  ASSERT_TRUE(lt.IsInitialized());
  EXPECT_EQ(lt.NumNodes(), 0);
  EXPECT_EQ(lt.NumEdges(), 0);
}

TEST(LoudsTreeTest, SingleNodeTest) {
  std::vector<size_t> tree{0};

  LoudsTree lt;
  ASSERT_TRUE(lt.Init(tree));
  ASSERT_TRUE(lt.IsInitialized());
  EXPECT_EQ(lt.NumNodes(), 1);
  EXPECT_EQ(lt.NumEdges(), 0);

  EXPECT_EQ(0, lt.NumChildren(0));
  EXPECT_EQ(std::numeric_limits<size_t>::max(), lt.FirstChild(0));
  EXPECT_EQ(std::numeric_limits<size_t>::max(), lt.LastChild(0));
}

TEST(LoudsTreeTest, PathTest) {
  // path graph
  std::vector<size_t> tree{1, 1, 1, 0};

  LoudsTree lt;
  ASSERT_TRUE(lt.Init(tree));
  ASSERT_TRUE(lt.IsInitialized());
  EXPECT_EQ(lt.NumNodes(), 4);
  EXPECT_EQ(lt.NumEdges(), 3);

  EXPECT_FALSE(lt.IsLeaf(0));
  EXPECT_EQ(1, lt.NumChildren(0));
  EXPECT_EQ(1, lt.FirstChild(0));
  EXPECT_EQ(1, lt.LastChild(0));
  EXPECT_EQ(std::numeric_limits<size_t>::max(), lt.Parent(0));

  EXPECT_FALSE(lt.IsLeaf(1));
  EXPECT_EQ(1, lt.NumChildren(1));
  EXPECT_EQ(2, lt.FirstChild(1));
  EXPECT_EQ(2, lt.LastChild(1));
  EXPECT_EQ(0, lt.Parent(1));

  EXPECT_FALSE(lt.IsLeaf(2));
  EXPECT_EQ(1, lt.NumChildren(2));
  EXPECT_EQ(3, lt.FirstChild(2));
  EXPECT_EQ(3, lt.LastChild(2));
  EXPECT_EQ(1, lt.Parent(2));

  EXPECT_TRUE(lt.IsLeaf(3));
  EXPECT_EQ(0, lt.NumChildren(3));
  EXPECT_EQ(std::numeric_limits<size_t>::max(), lt.FirstChild(3));
  EXPECT_EQ(std::numeric_limits<size_t>::max(), lt.LastChild(3));
  EXPECT_EQ(2, lt.Parent(3));
}

TEST(LoudsTreeTest, LongPathTest) {
  std::vector<size_t> tree;
  for (int i = 0; i < 10000; i++) {
    tree.push_back(1);
  }
  tree.push_back(0);

  LoudsTree lt;
  EXPECT_TRUE(lt.Init(tree));
  EXPECT_TRUE(lt.IsInitialized());
  EXPECT_EQ(10001, lt.NumNodes());
  EXPECT_EQ(10000, lt.NumEdges());
  EXPECT_FALSE(lt.IsLeaf(3));
  EXPECT_TRUE(lt.IsLeaf(10000));
}

TEST(LoudsTreeTest, LargeTreeTest) {
  std::vector<size_t> tree;
  for (int i = 0; i < (2 << 11) - (2 << 10) - 1; i++) {
    tree.push_back(2);
  }
  for (int i = 0; i < 2 << 10; i++) {
    tree.push_back(0);
  }

  LoudsTree lt;
  ASSERT_TRUE(lt.Init(tree));
  ASSERT_TRUE(lt.IsInitialized());
  EXPECT_EQ((2 << 11) - 1, lt.NumNodes());
  EXPECT_EQ((2 << 11) - 2, lt.NumEdges());
  EXPECT_FALSE(lt.IsLeaf(3));
  EXPECT_TRUE(lt.IsLeaf((2 << 11) - 2));
}

TEST(LoudsTreeTest, InitFail) {
  std::vector<size_t> tree{1, 0, 0, 3};
  LoudsTree lt;
  EXPECT_FALSE(lt.Init(tree));
  EXPECT_FALSE(lt.IsInitialized());

  tree = {1, 0, 0};
  EXPECT_FALSE(lt.Init(tree));
  EXPECT_FALSE(lt.IsInitialized());

  tree = {5};
  EXPECT_FALSE(lt.Init(tree));
  EXPECT_FALSE(lt.IsInitialized());
}

TEST(LoudsTreeTest, MultipleInits) {
  std::vector<size_t> tree{1, 1, 0};
  LoudsTree lt;
  lt.Init(tree);

  tree = {2, 0, 0};
  EXPECT_TRUE(lt.Init(tree));
  EXPECT_TRUE(lt.IsInitialized());
  EXPECT_EQ(lt.NumNodes(), 3);
  EXPECT_EQ(lt.NumEdges(), 2);

  tree = {1, 0, 0};
  EXPECT_FALSE(lt.Init(tree));
  EXPECT_FALSE(lt.IsInitialized());
  EXPECT_EQ(lt.NumNodes(), 3);
  EXPECT_EQ(lt.NumEdges(), 2);

  tree = {5, 0, 0};
  EXPECT_FALSE(lt.Init(tree));
  EXPECT_FALSE(lt.IsInitialized());
  EXPECT_EQ(lt.NumNodes(), 3);
  EXPECT_EQ(lt.NumEdges(), 2);

  tree = {5, 0, 0, 0, 0, 0};
  EXPECT_TRUE(lt.Init(tree));
  EXPECT_TRUE(lt.IsInitialized());
  EXPECT_EQ(lt.NumNodes(), 6);
  EXPECT_EQ(lt.NumEdges(), 5);
}

}  // namespace fst
