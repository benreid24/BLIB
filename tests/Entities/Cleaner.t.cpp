#include <BLIB/Entities.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace entity
{
namespace unittest
{
TEST(EntityCleaner, Disarmed) {
    Registry r;
    Entity e = InvalidEntity;

    {
        e = r.createEntity();
        Cleaner cleaner(r, e);
        cleaner.disarm();
    }
    r.doDestroy();

    EXPECT_TRUE(r.entityExists(e));
}

TEST(EntityCleaner, Armed) {
    Registry r;
    Entity e = InvalidEntity;

    {
        e = r.createEntity();
        Cleaner cleaner(r, e);
    }
    r.doDestroy();

    EXPECT_FALSE(r.entityExists(e));
}

} // namespace unittest
} // namespace entity
} // namespace bl
