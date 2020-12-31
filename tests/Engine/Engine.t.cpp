#include <BLIB/Engine.hpp>
#include <gtest/gtest.h>
#include <vector>

namespace bl
{
namespace unittest
{
class FlagTestState : public engine::State {
public:
    FlagTestState(engine::Flags::Flag flag)
    : callCount(0)
    , flag(flag) {}

    virtual void update(Engine& engine, float dt) {
        engine.flags().set(flag);
        ++callCount;
    }

    virtual void render(Engine& engine, float rd) {}

    int timesUpdated() const { return callCount; }

private:
    const engine::Flags::Flag flag;
    int callCount;
};

TEST(Engine, Terminate) {
    Engine engine({});

    FlagTestState* state = new FlagTestState(engine::Flags::Terminate);
    engine::State::Ptr ptr(state);
    EXPECT_EQ(engine.run(ptr), 1);
    EXPECT_EQ(state->timesUpdated(), 1);
    EXPECT_EQ(ptr.use_count(), 2);
}

TEST(Engine, PopState) {
    Engine engine({});

    FlagTestState* state = new FlagTestState(engine::Flags::PopState);
    engine::State::Ptr ptr(state);
    EXPECT_EQ(engine.run(ptr), 0);
    EXPECT_EQ(state->timesUpdated(), 1);
    EXPECT_EQ(ptr.use_count(), 1);
}

TEST(Engine, MultipleStates) {
    Engine engine({});

    FlagTestState* first = new FlagTestState(engine::Flags::PopState);
    engine::State::Ptr firstPtr(first);
    FlagTestState* second = new FlagTestState(engine::Flags::PopState);
    engine::State::Ptr secondPtr(second);

    // update -> pop -> next state -> update -> pop -> end
    engine.nextState(secondPtr);
    EXPECT_EQ(engine.run(firstPtr), 0);
    EXPECT_EQ(first->timesUpdated(), 1);
    EXPECT_EQ(second->timesUpdated(), 1);
}

class VariableTimeTestState : public engine::State {
public:
    VariableTimeTestState()
    : state(Constant)
    , counter(0) {
        times.reserve(6);
    }

    virtual void update(Engine& engine, float dt) {
        times.push_back(dt);
        switch (state) {
        case Constant:
            if (counter >= 1) {
                counter = 0;
                state   = Increasing;
                sf::sleep(sf::seconds(dt * 1.5f));
            }
            else {
                sf::sleep(sf::seconds(dt * 0.9f));
                ++counter;
            }
            break;

        case Increasing:
            if (counter >= 1) {
                counter = 0;
                state   = Decreasing;
                sf::sleep(sf::seconds(dt / 1.5f));
            }
            else {
                ++counter;
                sf::sleep(sf::seconds(dt * 1.5f));
            }
            break;

        case Decreasing:
            if (counter >= 1) { engine.flags().set(engine::Flags::Terminate); }
            else {
                ++counter;
                sf::sleep(sf::seconds(dt / 1.5f));
            }
            break;
        default:
            break;
        }
    }

    virtual void render(Engine& engine, float rd) {}

    const std::vector<float>& getTimes() const { return times; }

private:
    std::vector<float> times;
    enum State { Constant, Increasing, Decreasing } state;
    int counter;
};

TEST(Engine, VariableTimestep) {
    Engine engine(engine::Settings().withAllowVariableTimestep(true).withUpdateInterval(0.1f));

    VariableTimeTestState* state = new VariableTimeTestState();
    engine::State::Ptr ptr(state);
    EXPECT_EQ(engine.run(ptr), 1);
    ASSERT_GE(state->getTimes().size(), 6);

    EXPECT_EQ(state->getTimes().at(0), state->getTimes().at(1));
    EXPECT_LE(state->getTimes().at(1), state->getTimes().at(2));
    EXPECT_GE(state->getTimes().at(2), state->getTimes().at(3));
    EXPECT_GE(state->getTimes().at(3), state->getTimes().at(4));
    EXPECT_GE(state->getTimes().at(4), state->getTimes().at(5));
}

class FixedTimestepTestState : public engine::State {
public:
    virtual void update(Engine& engine, float dt) {
        times.push_back(dt);
        if (times.size() >= 10) engine.flags().set(engine::Flags::Terminate);
    }

    virtual void render(Engine& engine, float rd) {}

    const std::vector<float>& getTimes() const { return times; }

private:
    std::vector<float> times;
};

TEST(Engine, FixedTimestep) {
    Engine engine(engine::Settings().withAllowVariableTimestep(false));

    FixedTimestepTestState* state = new FixedTimestepTestState();
    engine::State::Ptr ptr(state);
    EXPECT_EQ(engine.run(ptr), 1);

    for (unsigned int i = 0; i < state->getTimes().size() - 1; ++i) {
        EXPECT_EQ(state->getTimes().at(i), state->getTimes().at(i + 1));
    }
}

} // namespace unittest
} // namespace bl
