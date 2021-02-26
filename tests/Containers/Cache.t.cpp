#include <BLIB/Containers/Cache.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace unittest
{
TEST(Cache, BasicElements) {
    Cache<int, int> cache;

    EXPECT_TRUE(cache.empty());
    cache.add(5, 5);
    cache.add(3, 6);
    EXPECT_EQ(cache.size(), 2);
    EXPECT_NE(cache.getForRead(5), nullptr);
    EXPECT_NE(cache.getForWrite(3), nullptr);
    EXPECT_EQ(cache.getForRead(7), nullptr);
    cache.erase(5);
    EXPECT_EQ(cache.getForRead(5), nullptr);
}

TEST(Cache, FifoEviction) {
    Cache<int, int> cache(Cache<int, int>::FirstInFirstOut, 1, 3);

    cache.add(1, 1);
    cache.add(2, 2);
    EXPECT_EQ(cache.size(), 2);
    cache.add(3, 3);
    EXPECT_EQ(cache.size(), 1);
    EXPECT_NE(cache.getForRead(3), nullptr);
}

TEST(Cache, ReadEviction) {
    Cache<int, int> cache(Cache<int, int>::LeastRecentlyAccessed, 2, 4);

    cache.add(1, 1);
    cache.add(2, 2);
    cache.add(3, 3);
    EXPECT_EQ(cache.size(), 3);
    cache.getForRead(1);
    cache.add(4, 4);
    EXPECT_EQ(cache.size(), 2);
    EXPECT_NE(cache.getForRead(1), nullptr);
    EXPECT_NE(cache.getForRead(4), nullptr);
}

TEST(Cache, WriteEviction) {
    Cache<int, int> cache(Cache<int, int>::LeastRecentlyModified, 2, 4);

    cache.add(1, 1);
    cache.add(2, 2);
    cache.add(3, 3);
    EXPECT_EQ(cache.size(), 3);
    cache.getForRead(1);
    cache.getForWrite(2);
    cache.add(4, 4);
    EXPECT_EQ(cache.size(), 2);
    EXPECT_NE(cache.getForRead(2), nullptr);
    EXPECT_NE(cache.getForRead(4), nullptr);
}

} // namespace unittest
} // namespace bl
