#include "TestRegistry.hpp"
#include <BLIB/ECS.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace ecs
{
namespace unittest
{
TEST(EntityCleaner, Disarmed) {
    testRegistry().destroyAllEntities();

    Entity e = InvalidEntity;
    {
        e = testRegistry().createEntity();
        Cleaner cleaner(testRegistry(), e);
        cleaner.disarm();
    }

    EXPECT_TRUE(testRegistry().entityExists(e));
}

TEST(EntityCleaner, Armed) {
    testRegistry().destroyAllEntities();

    Entity e = InvalidEntity;
    {
        e = testRegistry().createEntity();
        Cleaner cleaner(testRegistry(), e);
    }

    EXPECT_FALSE(testRegistry().entityExists(e));
}

} // namespace unittest
} // namespace ecs
} // namespace bl
