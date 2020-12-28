#include <BLIB/Engine/EngineFlags.hpp>
#include <gtest/gtest.h>

namespace bl
{
struct Engine {
    EngineFlags flags;
    void clear() { flags.clear(); }
};

namespace unittest
{
TEST(EngineFlags, FlagSet) {
    Engine engine;

    for (int flag = 0; flag < EngineFlags::_NUM_FLAGS; ++flag) {
        EXPECT_FALSE(engine.flags.flagSet(EngineFlags::Flag(flag)));
        engine.flags.setFlag(EngineFlags::Flag(flag));
        EXPECT_TRUE(engine.flags.flagSet(EngineFlags::Flag(flag)));
    }

    engine.clear();
    for (int flag = 0; flag < EngineFlags::_NUM_FLAGS; ++flag) {
        EXPECT_FALSE(engine.flags.flagSet(EngineFlags::Flag(flag)));
    }
}

} // namespace unittest
} // namespace bl
