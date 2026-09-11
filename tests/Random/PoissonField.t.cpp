#include <BLIB/Random/PoissonField.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace rand
{
namespace unittest
{
TEST(PoissonField, FollowsRules) {
    PoissonField field({50.f, 50.f}, 2.f);

    ASSERT_GE(field.samples().size(), 100);

    // bounds
    for (const glm::vec2& sample : field.samples()) {
        EXPECT_GE(sample.x, 0.f);
        EXPECT_GE(sample.y, 0.f);
        EXPECT_LE(sample.x, 50.f);
        EXPECT_LE(sample.y, 50.f);
    }

    // distance
    for (const glm::vec2& sample : field.samples()) {
        for (const glm::vec2& other : field.samples()) {
            if (&sample == &other) { continue; }
            const float dist = glm::distance(sample, other);
            EXPECT_GE(dist, 2.f);
        }
    }
}

} // namespace unittest
} // namespace rand
} // namespace bl
