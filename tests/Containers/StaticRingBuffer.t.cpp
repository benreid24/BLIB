#include <BLIB/Containers/StaticRingBuffer.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace ctr
{
namespace unittest
{
namespace
{
struct Dummy {
    Dummy()
    : d(nullptr) {}

    Dummy(Dummy&& c)
    : d(c.d) {
        c.d = nullptr;
    }

    Dummy(bool* d)
    : d(d) {}

    ~Dummy() {
        if (d) *d = true;
    }

    bool* d;
};
} // namespace

TEST(StaticRingBuffer, PushAndPop) {
    StaticRingBuffer<int, 5> buffer;

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

TEST(StaticRingBuffer, RollAround) {
    StaticRingBuffer<int, 2> buffer;

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

TEST(StaticRingBuffer, PopEmpty) {
    StaticRingBuffer<int, 2> buffer;

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

TEST(StaticRingBuffer, ClearDestructor) {
    StaticRingBuffer<Dummy, 2> buffer;
    bool dcalls[] = {false, false, false};

    buffer.emplace_back(&dcalls[0]);
    buffer.emplace_back(&dcalls[1]);
    Dummy d(&dcalls[2]);
    buffer.push_back(std::move(d));

    ASSERT_TRUE(dcalls[0]);
    ASSERT_FALSE(dcalls[1]);
    ASSERT_FALSE(dcalls[2]);

    EXPECT_EQ(buffer.size(), 2);
    buffer.clear();
    EXPECT_EQ(buffer.size(), 0);

    ASSERT_TRUE(dcalls[0]);
    ASSERT_TRUE(dcalls[1]);
    ASSERT_TRUE(dcalls[2]);
}

TEST(StaticRingBuffer, PushFront) {
    StaticRingBuffer<int, 2> buffer;

    buffer.push_back(5);
    buffer.push_front(10);
    EXPECT_EQ(buffer.front(), 10);
    EXPECT_EQ(buffer.back(), 5);
    buffer.clear();

    buffer.push_front(22);
    buffer.push_front(33);
    EXPECT_EQ(buffer.front(), 33);
    EXPECT_EQ(buffer.back(), 22);
    buffer.clear();

    buffer.push_front(55);
    buffer.push_front(66);
    buffer.push_front(99);
    EXPECT_EQ(buffer.front(), 99);
    EXPECT_EQ(buffer.back(), 66);
}

} // namespace unittest
} // namespace ctr
} // namespace bl
