#include <BLIB/Util/IdAllocatorUnbounded.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace util
{
namespace unittest
{
TEST(IdAllocatorUnbounded, AllocateAndRelease) {
    IdAllocatorUnbounded<unsigned int> alloc;

    ASSERT_EQ(alloc.allocate(), 0);
    ASSERT_EQ(alloc.allocate(), 1);
    ASSERT_EQ(alloc.allocate(), 2);
    ASSERT_EQ(alloc.allocate(), 3);

    for (int i = 0; i < 4; ++i) { EXPECT_TRUE(alloc.isAllocated(i)); }

    alloc.release(1);
    alloc.release(2);
    EXPECT_FALSE(alloc.isAllocated(1));
    EXPECT_FALSE(alloc.isAllocated(2));
    ASSERT_EQ(alloc.allocate(), 1);
    ASSERT_EQ(alloc.allocate(), 2);

    alloc.releaseAll();
    for (int i = 0; i < 4; ++i) { EXPECT_FALSE(alloc.isAllocated(i)); }
    ASSERT_EQ(alloc.allocate(), 0);
    ASSERT_EQ(alloc.allocate(), 1);
    ASSERT_EQ(alloc.allocate(), 2);
    ASSERT_EQ(alloc.allocate(), 3);
}

TEST(IdAllocatorUnbounded, ReAllocOrder) {
    IdAllocatorUnbounded<unsigned int> alloc(4);

    ASSERT_EQ(alloc.allocate(), 0);
    ASSERT_EQ(alloc.allocate(), 1);
    ASSERT_EQ(alloc.allocate(), 2);
    ASSERT_EQ(alloc.allocate(), 3);

    alloc.release(3);
    alloc.release(1);

    ASSERT_EQ(alloc.allocate(), 1);
    ASSERT_EQ(alloc.allocate(), 3);
}

} // namespace unittest
} // namespace util
} // namespace bl
