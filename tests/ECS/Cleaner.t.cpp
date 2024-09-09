#include <BLIB/ECS.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace ecs
{
namespace unittest
{
TEST(ECSEntityCleaner, Disarmed) {
    Registry testRegistry;

    Entity e = InvalidEntity;
    {
        e = testRegistry.createEntity(0);
        Cleaner cleaner(testRegistry, e);
        cleaner.disarm();
    }
    testRegistry.flushDeletions();
    EXPECT_TRUE(testRegistry.entityExists(e));
}

TEST(ECSEntityCleaner, Armed) {
    Registry testRegistry;

    Entity e = InvalidEntity;
    {
        e = testRegistry.createEntity(0);
        Cleaner cleaner(testRegistry, e);
    }
    testRegistry.flushDeletions();
    EXPECT_FALSE(testRegistry.entityExists(e));
}

} // namespace unittest
} // namespace ecs
} // namespace bl
