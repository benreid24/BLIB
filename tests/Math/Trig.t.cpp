#include <BLIB/Math/Trig.hpp>
#include <gtest/gtest.h>

#include <cmath>

namespace bl
{
namespace math
{
namespace unittest
{
#define FLOAT_COMPARE(expr1, expr2) EXPECT_LE(std::abs(expr1 - expr2), 0.0001)

TEST(Math, DegreesToRadians) {
    FLOAT_COMPARE(degreesToRadians(0.f), 0.f);
    FLOAT_COMPARE(degreesToRadians(90.f), Pi / 2.f);
    FLOAT_COMPARE(degreesToRadians(180.f), Pi);
    FLOAT_COMPARE(degreesToRadians(360.f), 2 * Pi);
}

TEST(Math, radiansToDegrees) {
    FLOAT_COMPARE(radiansToDegrees(0.f), 0.f);
    FLOAT_COMPARE(radiansToDegrees(Pi / 2.f), 90.f);
    FLOAT_COMPARE(radiansToDegrees(Pi), 180.f);
    FLOAT_COMPARE(radiansToDegrees(2 * Pi), 360.f);
}

TEST(Math, CosPositive) {
    FLOAT_COMPARE(cos(270.f), std::cos(degreesToRadians(270.f)));
    FLOAT_COMPARE(cos(123.5f), std::cos(degreesToRadians(123.5f)));
    FLOAT_COMPARE(cos(0.f), std::cos(degreesToRadians(0.f)));
    FLOAT_COMPARE(cos(892.25f), std::cos(degreesToRadians(892.25f)));
}

TEST(Math, CosNegative) {
    FLOAT_COMPARE(cos(-50.75f), std::cos(degreesToRadians(-50.75f)));
    FLOAT_COMPARE(cos(-456.75f), std::cos(degreesToRadians(-456.75f)));
}

TEST(Math, SinPositive) {
    FLOAT_COMPARE(sin(270.f), std::sin(degreesToRadians(270.f)));
    FLOAT_COMPARE(sin(123.5f), std::sin(degreesToRadians(123.5f)));
    FLOAT_COMPARE(sin(0.f), std::sin(degreesToRadians(0.f)));
    FLOAT_COMPARE(sin(892.25f), std::sin(degreesToRadians(892.25f)));
}

TEST(Math, SinNegative) {
    FLOAT_COMPARE(sin(-50.75f), std::sin(degreesToRadians(-50.75f)));
    FLOAT_COMPARE(sin(-456.75f), std::sin(degreesToRadians(-456.75f)));
}

} // namespace unittest
} // namespace math
} // namespace bl
