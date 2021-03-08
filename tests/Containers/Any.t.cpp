#include <BLIB/Containers/Any.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace container
{
namespace unittest
{
namespace
{
struct BigBoy {
    int more, than, eight, bytes, in, here;
};

struct DestructorBoy {
    DestructorBoy(int* killed)
    : killed(killed) {}

    ~DestructorBoy() { ++(*killed); }

    int* killed;
};

} // namespace

TEST(Any, OneType) {
    Any<8> any = 5;
    ASSERT_TRUE(any.hasValue());
    EXPECT_EQ(any.get<int>(), 5);

    any = 10;
    EXPECT_EQ(any.get<int>(), 10);
}

TEST(Any, DifferentTypes) {
    Any<8> any = std::string("hello world");
    ASSERT_TRUE(any.hasValue());
    EXPECT_EQ(any.get<std::string>(), "hello world");

    any = 42;
    EXPECT_EQ(any.get<int>(), 42);
}

TEST(Any, Allocations) {
    Any<8> any = 10;
    ASSERT_TRUE(any.hasValue());

    void* smallAddress = &any.get<int>();
    any                = BigBoy();
    void* bigAddress   = &any.get<BigBoy>();
    EXPECT_NE(smallAddress, bigAddress);
}

TEST(Any, Clear) {
    Any<8> any = 5;
    ASSERT_TRUE(any.hasValue());
    any.clear();
    EXPECT_FALSE(any.hasValue());
}

TEST(Any, CopyAssign) {
    Any<8> any1 = 42;
    Any<8> any2 = any1;
    EXPECT_EQ(any1.get<int>(), any2.get<int>());

    any2 = 55;
    EXPECT_EQ(any1.get<int>(), 42);
    EXPECT_EQ(any2.get<int>(), 55);
}

TEST(Any, BadAccess) {
    Any<8> any = 52;
    EXPECT_DEATH(any.get<std::string>(), "");
}

TEST(Any, Destructor) {
    int killed = 0;
    Any<16> any;
    any.emplace<DestructorBoy>(&killed);
    EXPECT_EQ(killed, 0);
    any = 42;
    EXPECT_EQ(killed, 1);
}

} // namespace unittest
} // namespace container
} // namespace bl
