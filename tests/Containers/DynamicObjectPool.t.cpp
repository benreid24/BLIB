#include <BLIB/Containers/Any.hpp>
#include <BLIB/Containers/DynamicObjectPool.hpp>
#include <gtest/gtest.h>
#include <unordered_set>

namespace bl
{
namespace container
{
namespace unittest
{
namespace
{
struct Dummy {
    Dummy()
    : d(nullptr) {}

    Dummy(bool* d)
    : d(d) {}

    Dummy(const Dummy& c)
    : d(c.d) {}

    Dummy(Dummy&& c)
    : d(c.d) {
        c.d = nullptr;
    }

    ~Dummy() {
        if (d) *d = true;
    }

    void set(bool* _d) { d = _d; }

    bool* d;
};

struct Data {
    int value;

    Data() = default;
    Data(int i)
    : value(i) {}
};

template<typename T>
typename DynamicObjectPool<T>::Iterator iterPos(DynamicObjectPool<T>& pool, unsigned int pos) {
    auto it = pool.begin();
    for (unsigned int i = 0; i < pos; ++i) { ++it; }
    return it;
}

} // namespace

TEST(DynamicObjectPool, Empty) {
    DynamicObjectPool<int> pool;
    EXPECT_TRUE(pool.empty());
}

TEST(DynamicObjectPool, AddIterator) {
    DynamicObjectPool<int> pool;
    pool.reserve(3);
    auto it1 = pool.add(1);
    auto it2 = pool.add(2);
    auto it3 = pool.add(3);
    EXPECT_EQ(*it1, 1);
    EXPECT_EQ(*it2, 2);
    EXPECT_EQ(*it3, 3);
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
    pool.erase(++pool.begin());

    ASSERT_EQ(pool.size(), 2);
    EXPECT_GE(pool.capacity(), 3);
    EXPECT_EQ(*pool.begin(), 5);
    EXPECT_EQ(*(++pool.begin()), 3);
}

TEST(DynamicObjectPool, IteratorSkipsDead) {
    DynamicObjectPool<int> pool;
    pool.add(1);
    pool.add(2);
    pool.add(3);
    pool.add(4);
    pool.add(5);

    pool.erase(iterPos(pool, 3));
    pool.erase(iterPos(pool, 1));

    auto it = pool.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 5);
}

TEST(DynamicObjectPool, IteratorEnd) {
    DynamicObjectPool<int> pool;
    EXPECT_EQ(pool.begin(), pool.end());
    pool.add(1);
    EXPECT_EQ(iterPos(pool, 1), pool.end());
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

    pool.erase(iterPos(pool, 3));
    pool.erase(iterPos(pool, 1));
    pool.add(10);
    pool.add(20);

    EXPECT_EQ(*pool.begin(), 1);
    EXPECT_EQ(*(iterPos(pool, 1)), 10);
    EXPECT_EQ(*(iterPos(pool, 2)), 3);
    EXPECT_EQ(*(iterPos(pool, 3)), 20);
    EXPECT_EQ(*(iterPos(pool, 4)), 5);
}

TEST(DynamicObjectPool, Destructor) {
    DynamicObjectPool<Dummy> pool;
    bool dcalls[] = {false, false, false};

    pool.add({});
    pool.add({});
    pool.add({});

    pool.begin()->set(&dcalls[0]);
    (++pool.begin())->set(&dcalls[1]);
    (iterPos(pool, 2))->set(&dcalls[2]);

    ASSERT_FALSE(dcalls[0]);
    ASSERT_FALSE(dcalls[1]);
    ASSERT_FALSE(dcalls[2]);

    pool.erase(iterPos(pool, 1));
    ASSERT_FALSE(dcalls[0]);
    ASSERT_TRUE(dcalls[1]);
    ASSERT_FALSE(dcalls[2]);

    pool.clear();
    ASSERT_EQ(pool.size(), 0);
    EXPECT_GE(pool.capacity(), 3);
    ASSERT_TRUE(dcalls[0]);
    ASSERT_TRUE(dcalls[1]);
    ASSERT_TRUE(dcalls[2]);

    pool.clear(true);
    EXPECT_EQ(pool.capacity(), 0);
}

TEST(DynamicObjectPool, Shrink) {
    DynamicObjectPool<int> pool;

    pool.add(1);
    pool.add(2);
    pool.add(3);
    pool.erase(iterPos(pool, 1));

    ASSERT_EQ(pool.size(), 2);
    ASSERT_GE(pool.capacity(), 3);

    pool.shrink();
    EXPECT_EQ(pool.capacity(), 2);
    EXPECT_EQ(*pool.begin(), 1);
    EXPECT_EQ(*(iterPos(pool, 1)), 3);
}

TEST(DynamicObjectPool, ShrinkDestructor) {
    DynamicObjectPool<Dummy> pool;
    pool.reserve(3);
    bool dcalls[] = {false, false, false};

    pool.add({});
    pool.add({});
    pool.add({});

    pool.begin()->set(&dcalls[0]);
    (++pool.begin())->set(&dcalls[1]);
    (iterPos(pool, 2))->set(&dcalls[2]);

    ASSERT_FALSE(dcalls[0]);
    ASSERT_FALSE(dcalls[1]);
    ASSERT_FALSE(dcalls[2]);

    pool.erase(iterPos(pool, 1));
    ASSERT_FALSE(dcalls[0]);
    ASSERT_TRUE(dcalls[1]);
    ASSERT_FALSE(dcalls[2]);

    pool.shrink();
    EXPECT_EQ(pool.capacity(), 2);

    ASSERT_FALSE(dcalls[0]);
    ASSERT_TRUE(dcalls[1]);
    ASSERT_FALSE(dcalls[2]);
}

TEST(DynamicObjectPool, EmplaceAndMove) {
    DynamicObjectPool<Dummy> pool;
    pool.reserve(2);
    bool dcalls[] = {false, false};

    Dummy d(&dcalls[0]);
    pool.add(std::move(d));
    pool.emplace(&dcalls[1]);

    ASSERT_FALSE(dcalls[0]);
    ASSERT_FALSE(dcalls[1]);

    pool.clear();
    EXPECT_TRUE(dcalls[0]);
    EXPECT_TRUE(dcalls[1]);
}

TEST(DynamicObjectPool, MultipleObjects) {
    DynamicObjectPool<Data> pool;

    pool.emplace(5);
    pool.emplace(10);
    pool.add({15});

    EXPECT_EQ(pool.begin()->value, 5);
    EXPECT_EQ((iterPos(pool, 1))->value, 10);
    EXPECT_EQ((iterPos(pool, 2))->value, 15);
}

TEST(DynamicObjectPool, Any) {
    DynamicObjectPool<Any<32>> pool;

    pool.add(Data(5));
    pool.add(Data(10));

    EXPECT_EQ(pool.begin()->get<Data>().value, 5);
    EXPECT_EQ((iterPos(pool, 1))->get<Data>().value, 10);
}

TEST(DynamicObjectPool, Iterate) {
    DynamicObjectPool<int> pool;

    pool.add(5);
    pool.emplace(15);
    pool.add(32);

    std::unordered_set<int> found;

    for (const int i : pool) { found.insert(i); }

    EXPECT_NE(found.find(5), found.end());
    EXPECT_NE(found.find(15), found.end());
    EXPECT_NE(found.find(32), found.end());
}

} // namespace unittest
} // namespace container
} // namespace bl
