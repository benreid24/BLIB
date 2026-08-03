#include <BLIB/Containers/InstrusiveLinkedList.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace ctr
{
namespace unittest
{

struct Node : public IntrusiveLinkedListNode {
    int value;

    Node()
    : value(0) {}

    explicit Node(int v)
    : value(v) {}

    Node(const Node& o)
    : value(o.value) {}
};

TEST(IntrusiveLinkedList, DefaultEmpty) {
    IntrusiveLinkedList<Node> list;
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0u);
    EXPECT_EQ(list.begin(), list.end());
}

TEST(IntrusiveLinkedList, PushBack) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.push_back(Node(2));
    list.push_back(Node(3));
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 3u);
    EXPECT_EQ(list.front().value, 1);
    EXPECT_EQ(list.back().value, 3);
}

TEST(IntrusiveLinkedList, PushFront) {
    IntrusiveLinkedList<Node> list;
    list.push_front(Node(3));
    list.push_front(Node(2));
    list.push_front(Node(1));
    EXPECT_EQ(list.size(), 3u);
    EXPECT_EQ(list.front().value, 1);
    EXPECT_EQ(list.back().value, 3);
}

TEST(IntrusiveLinkedList, EmplaceBack) {
    IntrusiveLinkedList<Node> list;
    list.emplace_back(10);
    list.emplace_back(20);
    EXPECT_EQ(list.size(), 2u);
    EXPECT_EQ(list.front().value, 10);
    EXPECT_EQ(list.back().value, 20);
}

TEST(IntrusiveLinkedList, EmplaceFront) {
    IntrusiveLinkedList<Node> list;
    list.emplace_front(20);
    list.emplace_front(10);
    EXPECT_EQ(list.size(), 2u);
    EXPECT_EQ(list.front().value, 10);
    EXPECT_EQ(list.back().value, 20);
}

TEST(IntrusiveLinkedList, InsertAtBegin) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(2));
    list.push_back(Node(3));
    list.insert(list.begin(), Node(1));
    EXPECT_EQ(list.size(), 3u);
    auto it = list.begin();
    EXPECT_EQ((*it).value, 1);
    ++it;
    EXPECT_EQ((*it).value, 2);
    ++it;
    EXPECT_EQ((*it).value, 3);
}

TEST(IntrusiveLinkedList, InsertAtEnd) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.push_back(Node(2));
    list.insert(list.end(), Node(3));
    EXPECT_EQ(list.size(), 3u);
    auto it = list.begin();
    EXPECT_EQ((*it).value, 1);
    ++it;
    EXPECT_EQ((*it).value, 2);
    ++it;
    EXPECT_EQ((*it).value, 3);
}

TEST(IntrusiveLinkedList, InsertInMiddle) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.push_back(Node(3));
    auto it = list.begin();
    ++it;
    list.insert(it, Node(2));
    EXPECT_EQ(list.size(), 3u);
    it = list.begin();
    EXPECT_EQ((*it).value, 1);
    ++it;
    EXPECT_EQ((*it).value, 2);
    ++it;
    EXPECT_EQ((*it).value, 3);
}

TEST(IntrusiveLinkedList, EmplaceInMiddle) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.push_back(Node(3));
    auto it = list.begin();
    ++it;
    list.emplace(it, 2);
    EXPECT_EQ(list.size(), 3u);
    it = list.begin();
    EXPECT_EQ((*it).value, 1);
    ++it;
    EXPECT_EQ((*it).value, 2);
    ++it;
    EXPECT_EQ((*it).value, 3);
}

TEST(IntrusiveLinkedList, EraseOnlyElement) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.erase(list.begin());
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0u);
    EXPECT_EQ(list.begin(), list.end());
}

TEST(IntrusiveLinkedList, EraseFront) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.push_back(Node(2));
    list.push_back(Node(3));
    auto it = list.erase(list.begin());
    EXPECT_EQ(list.size(), 2u);
    EXPECT_EQ(list.front().value, 2);
    EXPECT_EQ((*it).value, 2);
}

TEST(IntrusiveLinkedList, EraseBack) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.push_back(Node(2));
    list.push_back(Node(3));
    auto it = list.begin();
    ++it;
    ++it;
    auto next = list.erase(it);
    EXPECT_EQ(list.size(), 2u);
    EXPECT_EQ(list.back().value, 2);
    EXPECT_EQ(next, list.end());
}

TEST(IntrusiveLinkedList, EraseMiddle) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.push_back(Node(2));
    list.push_back(Node(3));
    auto it = list.begin();
    ++it;
    auto next = list.erase(it);
    EXPECT_EQ(list.size(), 2u);
    EXPECT_EQ((*next).value, 3);
    EXPECT_EQ(list.front().value, 1);
    EXPECT_EQ(list.back().value, 3);
}

TEST(IntrusiveLinkedList, Clear) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.push_back(Node(2));
    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0u);
}

TEST(IntrusiveLinkedList, IteratorForwardTraversal) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.push_back(Node(2));
    list.push_back(Node(3));
    int expected = 1;
    for (auto it = list.begin(); it != list.end(); ++it) { EXPECT_EQ((*it).value, expected++); }
    EXPECT_EQ(expected, 4);
}

TEST(IntrusiveLinkedList, IteratorBackwardTraversal) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.push_back(Node(2));
    list.push_back(Node(3));
    auto it = list.end();
    --it;
    EXPECT_EQ((*it).value, 3);
    --it;
    EXPECT_EQ((*it).value, 2);
    --it;
    EXPECT_EQ((*it).value, 1);
    EXPECT_EQ(it, list.begin());
}

TEST(IntrusiveLinkedList, IteratorPostIncrement) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.push_back(Node(2));
    auto it   = list.begin();
    auto prev = it++;
    EXPECT_EQ((*prev).value, 1);
    EXPECT_EQ((*it).value, 2);
}

TEST(IntrusiveLinkedList, IteratorPostDecrement) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.push_back(Node(2));
    auto it = list.begin();
    ++it;
    auto prev = it--;
    EXPECT_EQ((*prev).value, 2);
    EXPECT_EQ((*it).value, 1);
}

TEST(IntrusiveLinkedList, IteratorArrow) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(42));
    auto it = list.begin();
    EXPECT_EQ(it->value, 42);
}

TEST(IntrusiveLinkedList, CopyConstructor) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.push_back(Node(2));
    list.push_back(Node(3));
    IntrusiveLinkedList<Node> copy(list);
    EXPECT_EQ(copy.size(), 3u);
    auto it = copy.begin();
    EXPECT_EQ((*it).value, 1);
    ++it;
    EXPECT_EQ((*it).value, 2);
    ++it;
    EXPECT_EQ((*it).value, 3);
}

TEST(IntrusiveLinkedList, CopyAssignment) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.push_back(Node(2));
    IntrusiveLinkedList<Node> copy;
    copy.push_back(Node(99));
    copy = list;
    EXPECT_EQ(copy.size(), 2u);
    EXPECT_EQ(copy.front().value, 1);
    EXPECT_EQ(copy.back().value, 2);
}

TEST(IntrusiveLinkedList, MoveConstructor) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.push_back(Node(2));
    IntrusiveLinkedList<Node> moved(std::move(list));
    EXPECT_EQ(moved.size(), 2u);
    EXPECT_TRUE(list.empty());
}

TEST(IntrusiveLinkedList, MoveAssignment) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(1));
    list.push_back(Node(2));
    IntrusiveLinkedList<Node> moved;
    moved = std::move(list);
    EXPECT_EQ(moved.size(), 2u);
    EXPECT_TRUE(list.empty());
}

TEST(IntrusiveLinkedList, SpliceWholeList) {
    IntrusiveLinkedList<Node> dst;
    dst.push_back(Node(1));
    dst.push_back(Node(4));
    IntrusiveLinkedList<Node> src;
    src.push_back(Node(2));
    src.push_back(Node(3));
    auto it = dst.begin();
    ++it;
    dst.splice(it, src);
    EXPECT_TRUE(src.empty());
    EXPECT_EQ(dst.size(), 4u);
    it = dst.begin();
    EXPECT_EQ((*it).value, 1);
    ++it;
    EXPECT_EQ((*it).value, 2);
    ++it;
    EXPECT_EQ((*it).value, 3);
    ++it;
    EXPECT_EQ((*it).value, 4);
}

TEST(IntrusiveLinkedList, SpliceSingleNode) {
    IntrusiveLinkedList<Node> dst;
    dst.push_back(Node(1));
    dst.push_back(Node(3));
    IntrusiveLinkedList<Node> src;
    src.push_back(Node(2));
    src.push_back(Node(99));
    auto dstIt = dst.begin();
    ++dstIt;
    dst.splice(dstIt, src, src.begin());
    EXPECT_EQ(dst.size(), 3u);
    EXPECT_EQ(src.size(), 1u);
    auto it = dst.begin();
    EXPECT_EQ((*it).value, 1);
    ++it;
    EXPECT_EQ((*it).value, 2);
    ++it;
    EXPECT_EQ((*it).value, 3);
    EXPECT_EQ(src.front().value, 99);
    EXPECT_EQ(src.back().value, 99);
}

TEST(IntrusiveLinkedList, FrontBackSingleElement) {
    IntrusiveLinkedList<Node> list;
    list.push_back(Node(7));
    EXPECT_EQ(list.front().value, 7);
    EXPECT_EQ(list.back().value, 7);
}

TEST(IntrusiveLinkedList, IteratorReturnedByPushBack) {
    IntrusiveLinkedList<Node> list;
    auto it = list.push_back(Node(5));
    EXPECT_EQ((*it).value, 5);
}

TEST(IntrusiveLinkedList, IteratorReturnedByPushFront) {
    IntrusiveLinkedList<Node> list;
    auto it = list.push_front(Node(5));
    EXPECT_EQ((*it).value, 5);
}

} // namespace unittest
} // namespace ctr
} // namespace bl
