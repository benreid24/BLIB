#include <BLIB/Containers/DynamicObjectPool.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace unittest
{
TEST(DynamicObjectPool, Empty) {
    DynamicObjectPool<int> pool;
    EXPECT_TRUE(pool.empty());
}

TEST(DynamicObjectPool, SizeTracking) {
    DynamicObjectPool<int> pool;

    pool.add(5);
    pool.add(2);
    pool.add(3);
    EXPECT_EQ(pool.size(), 3);

    pool.erase(pool.begin());
    EXPECT_EQ(pool.size(), 2);
}

TEST(DynamicObjectPool, BasicErase) {
    DynamicObjectPool<int> pool;

    pool.add(5);
    pool.add(2);
    pool.add(3);
    pool.erase(pool.begin() + 1);

    ASSERT_EQ(pool.size(), 2);
    EXPECT_EQ(pool.capacity(), 3);
    EXPECT_EQ(*pool.begin(), 5);
    EXPECT_EQ(*(pool.begin() + 1), 3);
}

TEST(DynamicObjectPool, IteratorSkipsDead) {
    DynamicObjectPool<int> pool;
    pool.add(1);
    pool.add(2);
    pool.add(3);
    pool.add(4);
    pool.add(5);

    pool.erase(pool.begin() + 3);
    pool.erase(pool.begin() + 1);

    EXPECT_EQ(*pool.begin(), 1);
    EXPECT_EQ(*(pool.begin() + 1), 3);
    EXPECT_EQ(*(pool.begin() + 2), 5);
}

TEST(DynamicObjectPool, IteratorEnd) {
    DynamicObjectPool<int> pool;
    EXPECT_EQ(pool.begin(), pool.end());
    pool.add(1);
    EXPECT_EQ(pool.begin() + 1, pool.end());
    auto it = pool.begin();
    ++it;
    EXPECT_EQ(it, pool.end());
}

TEST(DynamicObjectPool, Reuse) {
    DynamicObjectPool<int> pool;
    pool.add(1);
    pool.add(2);
    pool.add(3);
    pool.add(4);
    pool.add(5);

    pool.erase(pool.begin() + 3);
    pool.erase(pool.begin() + 1);
    pool.add(10);
    pool.add(20);

    EXPECT_EQ(*pool.begin(), 1);
    EXPECT_EQ(*(pool.begin() + 1), 10);
    EXPECT_EQ(*(pool.begin() + 2), 3);
    EXPECT_EQ(*(pool.begin() + 3), 20);
    EXPECT_EQ(*(pool.begin() + 4), 5);
}

} // namespace unittest
} // namespace bl
