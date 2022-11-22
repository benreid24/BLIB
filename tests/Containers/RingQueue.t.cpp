#include <BLIB/Containers/RingQueue.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace container
{
namespace unittest
{
TEST(RingQueue, SizeAndWrapping) {
    RingQueue<int> queue(3);
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);
    EXPECT_EQ(queue.capacity(), 3);

    EXPECT_TRUE(queue.push(1));
    EXPECT_TRUE(queue.push(2));
    EXPECT_TRUE(queue.push(3));

    EXPECT_EQ(queue.size(), 3);
    EXPECT_EQ(queue.front(), 1);
    EXPECT_EQ(queue.back(), 3);
    queue.pop();

    EXPECT_EQ(queue.size(), 2);
    EXPECT_EQ(queue.front(), 2);
    EXPECT_EQ(queue.back(), 3);
    queue.pop();

    EXPECT_EQ(queue.size(), 1);
    EXPECT_EQ(queue.front(), 3);
    EXPECT_EQ(queue.back(), 3);
    queue.pop();

    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);

    // wrap around and verify size is still computed correctly
    EXPECT_TRUE(queue.push(3));
    EXPECT_TRUE(queue.push(4));
    EXPECT_TRUE(queue.push(5));
    queue.pop();
    EXPECT_TRUE(queue.push(6)); // should be in slot 0
    EXPECT_EQ(queue.size(), 3);
}

TEST(RingQueue, PushFail) {
    RingQueue<int> queue(2);
    EXPECT_TRUE(queue.push(1));
    EXPECT_TRUE(queue.push(1));
    EXPECT_FALSE(queue.push(1));
    EXPECT_EQ(queue.size(), 2);
}

TEST(RingQueue, Clear) {
    RingQueue<int> queue(4);

    // test w/o wrap around
    EXPECT_TRUE(queue.push(1));
    EXPECT_TRUE(queue.push(1));
    EXPECT_TRUE(queue.push(1));
    EXPECT_EQ(queue.size(), 3);
    queue.clear();
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);

    // test wrap around
    EXPECT_TRUE(queue.push(1));
    EXPECT_TRUE(queue.push(1));
    EXPECT_TRUE(queue.push(1));
    EXPECT_EQ(queue.size(), 3);
    queue.clear();
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);
}

} // namespace unittest
} // namespace container
} // namespace bl
