#include <BLIB/Engine.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace unittest
{
class TerminateState : public EngineState {
public:
    TerminateState()
    : callCount(0) {}

    virtual void update(Engine& engine, float dt) {
        engine.flags().setFlag(EngineFlags::Terminate);
        ++callCount;
    }

    virtual void render(Engine& engine, float rd) {}

    int timesUpdated() const { return callCount; }

private:
    int callCount;
};

TEST(Engine, Terminate) {
    sf::RenderWindow window;
    Engine engine(window, EngineSettings());

    TerminateState* state = new TerminateState();
    EngineState::Ptr ptr(state);
    EXPECT_EQ(engine.run(ptr), 1);
    EXPECT_EQ(state->timesUpdated(), 1);
}

} // namespace unittest
} // namespace bl