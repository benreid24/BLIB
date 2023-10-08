#include <BLIB/Util/RangeAllocatorUnbounded.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace util
{
namespace unittest
{
TEST(RangeAllocatorUnbounded, AllocAndRelease) {
    RangeAllocatorUnbounded<unsigned int> allocator(10);

    auto result1 = allocator.alloc(5);
    EXPECT_FALSE(result1.poolExpanded);
    EXPECT_EQ(result1.newPoolSize, allocator.poolSize());
    EXPECT_EQ(result1.range.size, 5);

    auto result2 = allocator.alloc(5);
    EXPECT_FALSE(result2.poolExpanded);
    EXPECT_EQ(result2.newPoolSize, allocator.poolSize());
    EXPECT_EQ(result2.range.size, 5);
    EXPECT_NE(result1.range.start, result2.range.start);

    allocator.release(result1.range);
    result1 = allocator.alloc(5);
    EXPECT_FALSE(result1.poolExpanded);
    EXPECT_EQ(result1.newPoolSize, allocator.poolSize());
    EXPECT_EQ(result1.range.size, 5);
    EXPECT_NE(result1.range.start, result2.range.start);

    auto result3 = allocator.alloc(5);
    EXPECT_TRUE(result3.poolExpanded);
    EXPECT_EQ(result3.newPoolSize, allocator.poolSize());
    EXPECT_EQ(result3.range.size, 5);
    EXPECT_NE(result3.range.start, result2.range.start);

    allocator.releaseAll();
    auto result4 = allocator.alloc(15);
    EXPECT_FALSE(result4.poolExpanded);
    EXPECT_EQ(result4.newPoolSize, allocator.poolSize());
    EXPECT_EQ(result4.range.size, 15);
}

TEST(RangeAllocatorUnbounded, BigAlloc) {
    RangeAllocatorUnbounded<unsigned int> allocator(10);

    auto result = allocator.alloc(11);
    EXPECT_TRUE(result.poolExpanded);
    EXPECT_EQ(result.newPoolSize, allocator.poolSize());
    EXPECT_EQ(result.range.size, 11);
    EXPECT_GE(allocator.poolSize(), 11);
}

TEST(RangeAllocatorUnbounded, VeryBigAlloc) {
    RangeAllocatorUnbounded<unsigned int> allocator(10);

    auto result = allocator.alloc(21);
    EXPECT_TRUE(result.poolExpanded);
    EXPECT_EQ(result.newPoolSize, allocator.poolSize());
    EXPECT_EQ(result.range.size, 21);
    EXPECT_GE(allocator.poolSize(), 21);
}

} // namespace unittest
} // namespace util
} // namespace bl
