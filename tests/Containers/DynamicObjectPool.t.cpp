#include <BLIB/Containers/DynamicObjectPool.hpp>
#include <BLIB/Containers/Any.hpp>
#include <gtest/gtest.h>

namespace bl
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
    Data(int i) : value(i) {}
};

} // namespace

TEST(DynamicObjectPool, Empty) {
    DynamicObjectPool<int> pool;
    EXPECT_TRUE(pool.empty());
}

TEST(DynamicObjectPool, AddIterator) {
    DynamicObjectPool<int> pool;
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

TEST(DynamicObjectPool, Destructor) {
    DynamicObjectPool<Dummy> pool;
    bool dcalls[] = {false, false, false};

    pool.add({});
    pool.add({});
    pool.add({});

    pool.begin()->set(&dcalls[0]);
    (++pool.begin())->set(&dcalls[1]);
    (pool.begin() + 2)->set(&dcalls[2]);

    ASSERT_FALSE(dcalls[0]);
    ASSERT_FALSE(dcalls[1]);
    ASSERT_FALSE(dcalls[2]);

    pool.erase(pool.begin() + 1);
    ASSERT_FALSE(dcalls[0]);
    ASSERT_TRUE(dcalls[1]);
    ASSERT_FALSE(dcalls[2]);

    pool.clear();
    ASSERT_EQ(pool.size(), 0);
    EXPECT_EQ(pool.capacity(), 3);
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
    pool.erase(pool.begin() + 1);

    ASSERT_EQ(pool.size(), 2);
    ASSERT_EQ(pool.capacity(), 3);

    pool.shrink();
    EXPECT_EQ(pool.capacity(), 2);
    EXPECT_EQ(*pool.begin(), 1);
    EXPECT_EQ(*(pool.begin() + 1), 3);
}

TEST(DynamicObjectPool, ShrinkDestructor) {
    DynamicObjectPool<Dummy> pool;
    bool dcalls[] = {false, false, false};

    pool.add({});
    pool.add({});
    pool.add({});

    pool.begin()->set(&dcalls[0]);
    (++pool.begin())->set(&dcalls[1]);
    (pool.begin() + 2)->set(&dcalls[2]);

    ASSERT_FALSE(dcalls[0]);
    ASSERT_FALSE(dcalls[1]);
    ASSERT_FALSE(dcalls[2]);

    pool.erase(pool.begin() + 1);
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
    EXPECT_EQ((pool.begin()+1)->value, 10);
    EXPECT_EQ((pool.begin()+2)->value, 15);
}

TEST(DynamicObjectPool, Any) {
    DynamicObjectPool<Any<32>> pool;

    pool.add(Data(5));
    pool.add(Data(10));

    Any<32>& temp = *pool.begin();
    auto val = temp.get<Data>();
    EXPECT_EQ(pool.begin()->get<Data>().value, 5);
    EXPECT_EQ((pool.begin()+1)->get<Data>().value, 10);
}

} // namespace unittest
} // namespace bl
