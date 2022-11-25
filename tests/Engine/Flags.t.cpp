#include <BLIB/Engine/Flags.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace engine
{
struct Engine {
    engine::Flags flags;
    void clear() { flags.clear(); }
};

namespace unittest
{
TEST(EngineFlags, FlagSet) {
    Engine engine;
    const std::array<Flags::Flag, 4> allFlags{
        Flags::Terminate, Flags::PopState, Flags::_priv_PushState, Flags::_priv_ReplaceState};

    for (const Flags::Flag flag : allFlags) {
        EXPECT_FALSE(engine.flags.active(engine::Flags::Flag(flag)));
        engine.flags.set(engine::Flags::Flag(flag));
        EXPECT_TRUE(engine.flags.active(engine::Flags::Flag(flag)));
    }

    engine.clear();
    for (const Flags::Flag flag : allFlags) {
        EXPECT_FALSE(engine.flags.active(engine::Flags::Flag(flag)));
    }
}

} // namespace unittest
} // namespace engine
} // namespace bl
