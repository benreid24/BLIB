#include <BLIB/Util/FastQueue.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace unittest
{
TEST(FastQueue, FifoTest) {
    FastQueue<int> queue;
    ASSERT_TRUE(queue.empty());

    queue.push_front(1);
    queue.push_front(2);
    queue.push_front(3);
    EXPECT_EQ(queue.size(), 3);
    EXPECT_EQ(queue.back(), 1);

    ASSERT_TRUE(queue.pop_back());
    EXPECT_EQ(queue.size(), 2);
    EXPECT_EQ(queue.back(), 2);
    ASSERT_TRUE(queue.pop_back());
    EXPECT_EQ(queue.size(), 1);
    EXPECT_EQ(queue.back(), 3);
    ASSERT_TRUE(queue.pop_back());
    EXPECT_EQ(queue.size(), 0);
}

TEST(FastQueue, ElementsUnique) {
    FastQueue<int> queue;
    queue.push_front(1);
    EXPECT_FALSE(queue.push_front(1));
    EXPECT_FALSE(queue.push_front(1));
    EXPECT_EQ(queue.size(), 1);
    EXPECT_EQ(queue.back(), 1);
}

TEST(FastQueue, ExistingElementPromote) {
    FastQueue<int> queue;
    queue.push_front(1);
    queue.push_front(2);
    queue.push_front(3);
    EXPECT_FALSE(queue.promote_front(4));
    EXPECT_TRUE(queue.promote_front(2));
    queue.pop_back();
    queue.pop_back();
    EXPECT_EQ(queue.back(), 2);
}

TEST(FastQueue, NewElementPromote) {
    FastQueue<int> queue;
    queue.push_front(1);
    queue.push_front(2);
    queue.push_front(3);
    EXPECT_TRUE(queue.promote_front(4, true));
    queue.pop_back();
    queue.pop_back();
    queue.pop_back();
    EXPECT_EQ(queue.back(), 4);
}

} // namespace unittest
} // namespace bl
