#include <BLIB/Containers/Any.hpp>
#include <BLIB/Containers/ObjectPool.hpp>
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
typename ObjectPool<T>::Iterator iterPos(ObjectPool<T>& pool, unsigned int pos) {
    auto it = pool.begin();
    for (unsigned int i = 0; i < pos; ++i) { ++it; }
    return it;
}

} // namespace

TEST(ObjectPool, Empty) {
    ObjectPool<int> pool(ObjectPool<int>::GrowthPolicy::ExpandBuffer, 1);
    EXPECT_TRUE(pool.empty());
}

TEST(ObjectPool, AddIterator) {
    ObjectPool<int> pool(ObjectPool<int>::GrowthPolicy::ExpandBuffer, 3);
    pool.reserve(3);
    auto it1 = pool.add(1);
    auto it2 = pool.add(2);
    auto it3 = pool.add(3);
    EXPECT_EQ(*it1, 1);
    EXPECT_EQ(*it2, 2);
    EXPECT_EQ(*it3, 3);
}

TEST(ObjectPool, SizeTracking) {
    ObjectPool<int> pool(ObjectPool<int>::GrowthPolicy::FailTerminate, 3);

    pool.add(5);
    pool.add(2);
    pool.add(3);
    EXPECT_EQ(pool.size(), 3);

    pool.erase(pool.begin());
    EXPECT_EQ(pool.size(), 2);
}

TEST(ObjectPool, BasicErase) {
    ObjectPool<int> pool(ObjectPool<int>::GrowthPolicy::FailContinue, 3);

    pool.add(5);
    pool.add(2);
    pool.add(3);
    pool.erase(++pool.begin());

    ASSERT_EQ(pool.size(), 2);
    EXPECT_EQ(pool.capacity(), 3);
    EXPECT_EQ(*pool.begin(), 5);
    EXPECT_EQ(*(++pool.begin()), 3);
}

TEST(ObjectPool, IteratorSkipsDead) {
    ObjectPool<int> pool(ObjectPool<int>::GrowthPolicy::ExpandBuffer, 1);
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

TEST(ObjectPool, IteratorEnd) {
    ObjectPool<int> pool(ObjectPool<int>::GrowthPolicy::ExpandBuffer, 1);
    EXPECT_EQ(pool.begin(), pool.end());
    pool.add(1);
    EXPECT_EQ(iterPos(pool, 1), pool.end());
    auto it = pool.begin();
    ++it;
    EXPECT_EQ(it, pool.end());
}

TEST(ObjectPool, Reuse) {
    ObjectPool<int> pool(ObjectPool<int>::GrowthPolicy::ExpandBuffer, 5);
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

TEST(ObjectPool, Destructor) {
    ObjectPool<Dummy> pool(ObjectPool<Dummy>::GrowthPolicy::ExpandBuffer, 3);
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

TEST(ObjectPool, Shrink) {
    ObjectPool<int> pool(ObjectPool<int>::GrowthPolicy::ExpandBuffer, 3);

    pool.add(1);
    pool.add(2);
    pool.add(3);
    pool.erase(iterPos(pool, 1));

    ASSERT_EQ(pool.size(), 2);
    ASSERT_EQ(pool.capacity(), 3);

    pool.shrink();
    EXPECT_EQ(pool.capacity(), 2);
    EXPECT_EQ(*pool.begin(), 1);
    EXPECT_EQ(*(iterPos(pool, 1)), 3);
}

TEST(ObjectPool, ShrinkDestructor) {
    ObjectPool<Dummy> pool(ObjectPool<Dummy>::GrowthPolicy::ExpandBuffer, 3);
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

TEST(ObjectPool, EmplaceAndMove) {
    ObjectPool<Dummy> pool(ObjectPool<Dummy>::GrowthPolicy::FailTerminate, 2);
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

TEST(ObjectPool, MultipleObjects) {
    ObjectPool<Data> pool(ObjectPool<Data>::GrowthPolicy::ExpandBuffer, 3);

    pool.emplace(5);
    pool.emplace(10);
    pool.add({15});

    EXPECT_EQ(pool.begin()->value, 5);
    EXPECT_EQ((iterPos(pool, 1))->value, 10);
    EXPECT_EQ((iterPos(pool, 2))->value, 15);
}

TEST(ObjectPool, Any) {
    ObjectPool<Any<32>> pool(ObjectPool<Any<32>>::GrowthPolicy::ExpandBuffer, 1);

    pool.add(Data(5));
    pool.add(Data(10));

    EXPECT_EQ(pool.begin()->get<Data>().value, 5);
    EXPECT_EQ((iterPos(pool, 1))->get<Data>().value, 10);
}

TEST(ObjectPool, Iterate) {
    ObjectPool<int> pool(ObjectPool<int>::GrowthPolicy::FailContinue, 3);

    pool.add(5);
    pool.emplace(15);
    pool.add(32);

    std::unordered_set<int> found;

    for (const int i : pool) { found.insert(i); }

    EXPECT_NE(found.find(5), found.end());
    EXPECT_NE(found.find(15), found.end());
    EXPECT_NE(found.find(32), found.end());
}

TEST(ObjectPool, FullContinue) {
    ObjectPool<int> pool(ObjectPool<int>::GrowthPolicy::FailContinue, 2);
    auto it = pool.add(5);
    EXPECT_NE(it, pool.end());
    it = pool.add(5);
    EXPECT_NE(it, pool.end());
    it = pool.add(5);
    EXPECT_EQ(it, pool.end());
}

TEST(ObjectPool, FullTerminate) {
    ObjectPool<int> pool(ObjectPool<int>::GrowthPolicy::FailTerminate, 2);
    auto it = pool.add(5);
    EXPECT_NE(it, pool.end());
    it = pool.add(5);
    EXPECT_NE(it, pool.end());
    ASSERT_DEATH(pool.add(5), "");
}

} // namespace unittest
} // namespace container
} // namespace bl
