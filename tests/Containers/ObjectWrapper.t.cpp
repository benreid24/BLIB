#include <BLIB/Containers/ObjectWrapper.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace ctr
{
namespace unittest
{
namespace
{
struct Object {
    int data;

    Object()
    : data(55) {}

    Object(int a, int b)
    : data(a * b) {}
};

struct Destroyer {
    Destroyer(int& cc)
    : cc(cc) {}

    ~Destroyer() { ++cc; }

    int& cc;
};

} // namespace

TEST(ObjectWrapper, UnInitialized) {
    ObjectWrapper<Object> obj;
    EXPECT_NE(obj.get().data, 55);
}

TEST(ObjectWrapper, ConstructAndEmplace) {
    ObjectWrapper<Object> obj1(std::in_place, 5, 2);
    EXPECT_EQ(obj1.get().data, 10);

    ObjectWrapper<Object> obj2;
    obj2.emplace(6, 6);
    EXPECT_EQ(obj2.get().data, 36);
}

TEST(ObjectWrapper, Destroy) {
    int cc = 0;
    {
        ObjectWrapper<Destroyer> obj;
        obj.emplace(std::ref(cc));
        obj.destroy();
    }
    EXPECT_EQ(cc, 1);
}

} // namespace unittest
} // namespace ctr
} // namespace bl
