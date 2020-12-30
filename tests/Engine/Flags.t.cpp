#include <BLIB/Engine/Flags.hpp>
#include <gtest/gtest.h>

namespace bl
{
struct Engine {
    engine::Flags flags;
    void clear() { flags.clear(); }
};

namespace unittest
{
TEST(EngineFlags, FlagSet) {
    Engine engine;

    for (int flag = 0; flag < engine::Flags::_NUM_FLAGS; ++flag) {
        EXPECT_FALSE(engine.flags.active(engine::Flags::Flag(flag)));
        engine.flags.set(engine::Flags::Flag(flag));
        EXPECT_TRUE(engine.flags.active(engine::Flags::Flag(flag)));
    }

    engine.clear();
    for (int flag = 0; flag < engine::Flags::_NUM_FLAGS; ++flag) {
        EXPECT_FALSE(engine.flags.active(engine::Flags::Flag(flag)));
    }
}

} // namespace unittest
} // namespace bl
