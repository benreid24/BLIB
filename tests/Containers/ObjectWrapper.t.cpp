#include <BLIB/Containers/ObjectWrapper.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace container
{
namespace unittest
{
namespace
{
struct TestObject {
    TestObject(int* dt, int v)
    : dtorCalled(dt)
    , data(v) {}

    TestObject(const TestObject&) = default;

    TestObject(TestObject&& mv)
    : dtorCalled(mv.dtorCalled)
    , data(mv.data) {
        mv.dtorCalled = nullptr;
    }

    ~TestObject() {
        if (dtorCalled != nullptr) *dtorCalled += 1;
    }

    TestObject& operator=(const TestObject&) = default;

    TestObject& operator=(TestObject&& o) {
        dtorCalled   = o.dtorCalled;
        data         = o.data;
        o.dtorCalled = nullptr;
        return *this;
    }

    int* dtorCalled;
    int data;
};
} // namespace

TEST(ObjectWrapper, Emlplace) {
    ObjectWrapper<TestObject> wrapper;

    ASSERT_FALSE(wrapper.hasValue());
    int dtor = 0;
    wrapper.emplace(&dtor, 56);
    ASSERT_TRUE(wrapper.hasValue());
    EXPECT_EQ(wrapper.get().data, 56);
    wrapper.destroy();
    ASSERT_FALSE(wrapper.hasValue());
    EXPECT_EQ(dtor, 1);
}

TEST(ObjectWrapper, EmplaceDestruct) {
    int dtor = 0;
    ObjectWrapper<TestObject> wrapper(&dtor, 105);
    wrapper.emplace(&dtor, -123);

    EXPECT_EQ(wrapper.get().data, -123);
    EXPECT_EQ(dtor, 1);
    EXPECT_TRUE(wrapper.hasValue());
}

TEST(ObjectWrapper, AssignValue) {
    ObjectWrapper<TestObject> wrapper;

    int dtor = 0;
    {
        TestObject obj(&dtor, 99);
        wrapper.assign(obj);
        EXPECT_EQ(wrapper.get().data, 99);
    }

    EXPECT_EQ(dtor, 1);
    wrapper.destroy();
    EXPECT_EQ(dtor, 2);
}

TEST(ObjectWrapper, MoveValue) {
    ObjectWrapper<TestObject> wrapper;

    int dtor = 0;
    {
        TestObject obj(&dtor, 99);
        wrapper.move(std::forward<TestObject>(obj));
        EXPECT_EQ(wrapper.get().data, 99);
    }

    EXPECT_EQ(dtor, 0);
    wrapper.destroy();
    EXPECT_EQ(dtor, 1);
}

TEST(ObjectWrapper, OverwriteCopy) {
    ObjectWrapper<TestObject> wrapper;
    int dtor = 0;
    wrapper.emplace(&dtor, 85);

    {
        TestObject obj(&dtor, 99);
        wrapper.assign(obj);
        EXPECT_EQ(wrapper.get().data, 99);
    }

    EXPECT_EQ(dtor, 1);
    wrapper.destroy();
    EXPECT_EQ(dtor, 2);
}

TEST(ObjectWrapper, OverwriteMove) {
    ObjectWrapper<TestObject> wrapper;
    int dtor = 0;
    wrapper.emplace(&dtor, 85);

    {
        TestObject obj(&dtor, 99);
        wrapper.move(std::forward<TestObject>(obj));
        EXPECT_EQ(wrapper.get().data, 99);
    }

    EXPECT_EQ(dtor, 0);
    wrapper.destroy();
    EXPECT_EQ(dtor, 1);
}

TEST(ObjectWrapper, WrapperAssign) {
    int dtor = 0;
    {
        ObjectWrapper<TestObject> w1(&dtor, 99);
        ObjectWrapper<TestObject> w2;
        w2 = w1;

        EXPECT_EQ(w1.get().data, 99);
        EXPECT_EQ(w2.get().data, 99);
    }
    EXPECT_EQ(dtor, 2);
}

TEST(ObjectWrapper, WrapperMove) {
    int dtor = 0;
    {
        ObjectWrapper<TestObject> w1(&dtor, 99);
        ObjectWrapper<TestObject> w2;
        w2 = std::forward<ObjectWrapper<TestObject>>(w1);

        EXPECT_FALSE(w1.hasValue());
        EXPECT_EQ(w2.get().data, 99);
    }
    EXPECT_EQ(dtor, 1);
}

} // namespace unittest
} // namespace container
} // namespace bl
