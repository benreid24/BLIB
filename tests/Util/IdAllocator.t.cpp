#include <BLIB/Util/IdAllocator.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace util
{
namespace unittest
{
TEST(IdAllocator, AllocateAndRelease) {
    IdAllocator<unsigned int> alloc(4);

    ASSERT_TRUE(alloc.available());
    ASSERT_EQ(alloc.allocate(), 0);
    ASSERT_EQ(alloc.allocate(), 1);
    ASSERT_EQ(alloc.allocate(), 2);
    ASSERT_EQ(alloc.allocate(), 3);
    EXPECT_FALSE(alloc.available());

    for (int i = 0; i < 4; ++i) { EXPECT_TRUE(alloc.allocatedIds()[i]); }

    alloc.release(1);
    alloc.release(2);
    ASSERT_TRUE(alloc.available());
    EXPECT_FALSE(alloc.allocatedIds()[1]);
    EXPECT_FALSE(alloc.allocatedIds()[2]);
    ASSERT_EQ(alloc.allocate(), 1);
    ASSERT_EQ(alloc.allocate(), 2);
    EXPECT_FALSE(alloc.available());

    alloc.releaseAll();
    ASSERT_TRUE(alloc.available());
    for (int i = 0; i < 4; ++i) { EXPECT_FALSE(alloc.allocatedIds()[i]); }
    ASSERT_EQ(alloc.allocate(), 0);
    ASSERT_EQ(alloc.allocate(), 1);
    ASSERT_EQ(alloc.allocate(), 2);
    ASSERT_EQ(alloc.allocate(), 3);
    EXPECT_FALSE(alloc.available());
}

TEST(IdAllocator, HighIdTracking) {
    IdAllocator<unsigned int> alloc(4);

    ASSERT_TRUE(alloc.available());
    ASSERT_EQ(alloc.allocate(), 0);
    ASSERT_EQ(alloc.allocate(), 1);
    ASSERT_EQ(alloc.allocate(), 2);
    ASSERT_EQ(alloc.allocate(), 3);

    EXPECT_EQ(alloc.highestId(), 3);
    alloc.release(1);
    EXPECT_EQ(alloc.highestId(), 3);
    alloc.release(2);
    EXPECT_EQ(alloc.highestId(), 3);
    alloc.release(3);
    EXPECT_EQ(alloc.highestId(), 0);

    alloc.releaseAll();
    ASSERT_EQ(alloc.allocate(), 0);
    ASSERT_EQ(alloc.allocate(), 1);
    ASSERT_EQ(alloc.allocate(), 2);
    ASSERT_EQ(alloc.allocate(), 3);
    EXPECT_EQ(alloc.highestId(), 3);
    alloc.release(3);
    EXPECT_EQ(alloc.highestId(), 2);
    alloc.release(2);
    EXPECT_EQ(alloc.highestId(), 1);
    alloc.release(1);
    EXPECT_EQ(alloc.highestId(), 0);
}

} // namespace unittest
} // namespace util
} // namespace bl
