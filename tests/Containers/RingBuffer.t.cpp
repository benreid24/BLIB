#include <BLIB/Containers/RingBuffer.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace unittest
{
TEST(RingBuffer, PushAndPop) {
    RingBuffer<int> buffer(5);

    ASSERT_TRUE(buffer.empty());
    ASSERT_FALSE(buffer.full());
    ASSERT_EQ(buffer.size(), 0);
    ASSERT_EQ(buffer.capacity(), 5);

    buffer.push_back(5);
    buffer.push_back(10);

    ASSERT_FALSE(buffer.empty());
    ASSERT_FALSE(buffer.full());
    ASSERT_EQ(buffer.size(), 2);
    EXPECT_EQ(buffer[0], 5);
    EXPECT_EQ(buffer[1], 10);
    EXPECT_EQ(buffer.front(), 5);
    EXPECT_EQ(buffer.back(), 10);

    buffer.pop_front();
    ASSERT_FALSE(buffer.empty());
    ASSERT_FALSE(buffer.full());
    ASSERT_EQ(buffer.size(), 1);
    EXPECT_EQ(buffer[0], 10);
    EXPECT_EQ(buffer.front(), 10);
    EXPECT_EQ(buffer.back(), 10);
}

TEST(RingBuffer, RollAround) {
    RingBuffer<int> buffer(2);

    ASSERT_TRUE(buffer.empty());
    ASSERT_FALSE(buffer.full());
    ASSERT_EQ(buffer.size(), 0);
    ASSERT_EQ(buffer.capacity(), 2);

    buffer.push_back(5); // falls off
    buffer.push_back(10);
    buffer.push_back(7);

    ASSERT_FALSE(buffer.empty());
    ASSERT_TRUE(buffer.full());
    ASSERT_EQ(buffer.size(), 2);
    EXPECT_EQ(buffer[0], 10);
    EXPECT_EQ(buffer[1], 7);
    EXPECT_EQ(buffer.front(), 10);
    EXPECT_EQ(buffer.back(), 7);

    buffer.pop_front();
    ASSERT_FALSE(buffer.empty());
    ASSERT_FALSE(buffer.full());
    ASSERT_EQ(buffer.size(), 1);
    EXPECT_EQ(buffer[0], 7);
    EXPECT_EQ(buffer.front(), 7);
    EXPECT_EQ(buffer.back(), 7);
}

TEST(RingBuffer, PopEmpty) {
    RingBuffer<int> buffer(2);

    buffer.push_back(5); // falls off
    buffer.push_back(10);
    buffer.push_back(7);
    buffer.pop_front();
    buffer.pop_front();
    buffer.pop_front();

    ASSERT_TRUE(buffer.empty());
    ASSERT_FALSE(buffer.full());
    ASSERT_EQ(buffer.size(), 0);
    ASSERT_EQ(buffer.capacity(), 2);

    buffer.push_back(5); // falls off
    buffer.push_back(10);
    buffer.push_back(7);

    ASSERT_FALSE(buffer.empty());
    ASSERT_TRUE(buffer.full());
    ASSERT_EQ(buffer.size(), 2);
    EXPECT_EQ(buffer[0], 10);
    EXPECT_EQ(buffer[1], 7);
    EXPECT_EQ(buffer.front(), 10);
    EXPECT_EQ(buffer.back(), 7);
}

} // namespace unittest
} // namespace bl
