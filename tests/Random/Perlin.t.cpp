#include <BLIB/Random/Perlin.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace rand
{
namespace unittest
{
TEST(Perlin, Seed) {
    Perlin<float> p1(12345);
    Perlin<float> p2(12345);

    EXPECT_FLOAT_EQ(p1.noise1D(0.5f), p2.noise1D(0.5f));
    EXPECT_FLOAT_EQ(p1.noise2D(0.5f, 0.5f), p2.noise2D(0.5f, 0.5f));
    EXPECT_FLOAT_EQ(p1.noise3D(0.5f, 0.5f, 0.5f), p2.noise3D(0.5f, 0.5f, 0.5f));
    EXPECT_FLOAT_EQ(p1.octave1D(0.5f, 4, 0.5f), p2.octave1D(0.5f, 4, 0.5f));
    EXPECT_FLOAT_EQ(p1.octave2D(0.5f, 0.5f, 4, 0.5f), p2.octave2D(0.5f, 0.5f, 4, 0.5f));
    EXPECT_FLOAT_EQ(p1.octave3D(0.5f, 0.5f, 0.5f, 4, 0.5f), p2.octave3D(0.5f, 0.5f, 0.5f, 4, 0.5f));
    EXPECT_FLOAT_EQ(p1.octave1DNormalized(0.5f, 4, 0.5f), p2.octave1DNormalized(0.5f, 4, 0.5f));
    EXPECT_FLOAT_EQ(p1.octave2DNormalized(0.5f, 0.5f, 4, 0.5f),
                    p2.octave2DNormalized(0.5f, 0.5f, 4, 0.5f));
    EXPECT_FLOAT_EQ(p1.octave3DNormalized(0.5f, 0.5f, 0.5f, 4, 0.5f),
                    p2.octave3DNormalized(0.5f, 0.5f, 0.5f, 4, 0.5f));
}

} // namespace unittest
} // namespace rand
} // namespace bl
