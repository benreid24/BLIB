#include <BLIB/Components/Hitbox2D.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace com
{
namespace unittest
{
TEST(Hitbox2D, Point) {
    Transform2D transform({50.f, 50.f});
    Hitbox2D testBox(&transform);

    EXPECT_FALSE(testBox.containsPoint({50.f, 50.f}));

    EXPECT_TRUE(testBox.intersectsCircle({45.f, 49.f}, 15.f));
    EXPECT_FALSE(testBox.intersectsCircle({100.f, 100.f}, 20.f));

    EXPECT_TRUE(testBox.intersectsRect({40.f, 30.f}, {20.f, 30.f}));
    EXPECT_FALSE(testBox.intersectsRect({100.f, 100.f}, {100.f, 100.f}));
}

TEST(Hitbox2D, Circle) {
    Transform2D transform({50.f, 50.f});
    Hitbox2D testBox(&transform, 10.f);

    EXPECT_FALSE(testBox.containsPoint({160.f, 60.f}));
    EXPECT_TRUE(testBox.containsPoint({50.f, 55.f}));

    EXPECT_TRUE(testBox.intersectsCircle({50.f, 74.f}, 15.f));
    EXPECT_FALSE(testBox.intersectsCircle({100.f, 100.f}, 20.f));

    EXPECT_TRUE(testBox.intersectsRect({45.f, 55.f}, {10.f, 10.f}));
    EXPECT_TRUE(testBox.intersectsRect({56.f, 36.f}, {10.f, 10.f}));
    EXPECT_FALSE(testBox.intersectsRect({100.f, 100.f}, {100.f, 100.f}));
}

TEST(Hitbox2D, Rectangle) {
    Transform2D transform({50.f, 50.f});
    Hitbox2D testBox(&transform, {20.f, 20.f});

    EXPECT_FALSE(testBox.containsPoint({160.f, 60.f}));
    EXPECT_TRUE(testBox.containsPoint({60.f, 60.f}));

    EXPECT_TRUE(testBox.intersectsCircle({45.f, 45.f}, 10.f));
    EXPECT_TRUE(testBox.intersectsCircle({55.f, 55.f}, 10.f));
    EXPECT_FALSE(testBox.intersectsCircle({100.f, 100.f}, 20.f));

    EXPECT_TRUE(testBox.intersectsRect({40.f, 30.f}, {20.f, 30.f}));
    EXPECT_FALSE(testBox.intersectsRect({100.f, 100.f}, {100.f, 100.f}));
}

} // namespace unittest
} // namespace com
} // namespace bl
