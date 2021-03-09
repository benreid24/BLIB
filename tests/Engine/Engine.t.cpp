#include <BLIB/Engine.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Logging.hpp>
#include <gtest/gtest.h>
#include <vector>

namespace bl
{
namespace engine
{
namespace unittest
{
class FlagTestState : public State {
public:
    FlagTestState(Flags::Flag flag)
    : callCount(0)
    , flag(flag) {}

    virtual void update(Engine& engine, float dt) {
        engine.flags().set(flag);
        ++callCount;
    }

    virtual void render(Engine& engine, float rd) {}

    int timesUpdated() const { return callCount; }

    virtual const char* name() const override { return "FlagTestState"; }

private:
    const Flags::Flag flag;
    int callCount;
};

TEST(Engine, Terminate) {
    Engine engine(Settings().withCreateWindow(false));

    FlagTestState* state = new FlagTestState(Flags::Terminate);
    State::Ptr ptr(state);
    EXPECT_EQ(engine.run(ptr), true);
    EXPECT_EQ(state->timesUpdated(), 1);
    EXPECT_EQ(ptr.use_count(), 2);
}

TEST(Engine, PopState) {
    Engine engine(Settings().withCreateWindow(false));

    FlagTestState* state = new FlagTestState(Flags::PopState);
    State::Ptr ptr(state);
    EXPECT_EQ(engine.run(ptr), true);
    EXPECT_EQ(state->timesUpdated(), 1);
    EXPECT_EQ(ptr.use_count(), 1);
}

TEST(Engine, MultipleStates) {
    Engine engine(Settings().withCreateWindow(false));

    FlagTestState* first = new FlagTestState(Flags::PopState);
    State::Ptr firstPtr(first);
    FlagTestState* second = new FlagTestState(Flags::PopState);
    State::Ptr secondPtr(second);

    // update -> pop -> next state -> update -> pop -> end
    engine.nextState(secondPtr);
    EXPECT_EQ(engine.run(firstPtr), true);
    EXPECT_EQ(first->timesUpdated(), 1);
    EXPECT_EQ(second->timesUpdated(), 1);
}

class VariableTimeTestState : public State {
public:
    VariableTimeTestState()
    : state(Constant)
    , counter(0) {
        times.reserve(6);
    }

    virtual const char* name() const override { return "VariableTimeTestState"; }

    virtual void update(Engine& engine, float dt) {
        BL_LOG_INFO << "update() called with timestep: " << dt << "s";
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
                sf::sleep(sf::seconds(dt / 3.f));
            }
            else {
                ++counter;
                sf::sleep(sf::seconds(dt * 2.f));
            }
            break;

        case Decreasing:
            if (counter >= 1) { engine.flags().set(Flags::Terminate); }
            else {
                ++counter;
                sf::sleep(sf::seconds(dt / 1.5f));
            }
            break;
        default:
            break;
        }
    }

    virtual void render(Engine& engine, float rd) {
        sf::sleep(sf::seconds(0.05));
        BL_LOG_INFO << "render() called with residual lag: " << rd << "s";
    }

    const std::vector<float>& getTimes() const { return times; }

private:
    std::vector<float> times;
    enum State { Constant, Increasing, Decreasing } state;
    int counter;
};

TEST(Engine, VariableTimestep) {
    Engine engine(
        Settings().withAllowVariableTimestep(true).withUpdateInterval(0.1f).withCreateWindow(
            false));

    VariableTimeTestState* state = new VariableTimeTestState();
    State::Ptr ptr(state);
    EXPECT_EQ(engine.run(ptr), true);
    ASSERT_GE(state->getTimes().size(), 6);

    EXPECT_EQ(state->getTimes().at(0), state->getTimes().at(1));
    EXPECT_LE(state->getTimes().at(1), state->getTimes().at(2));
    EXPECT_LE(state->getTimes().at(2), state->getTimes().at(3));
    EXPECT_GE(state->getTimes().at(3), state->getTimes().at(4));
    EXPECT_GE(state->getTimes().at(4), state->getTimes().at(5));
}

class FixedTimestepTestState : public State {
public:
    virtual void update(Engine& engine, float dt) {
        times.push_back(dt);
        if (times.size() >= 10) engine.flags().set(Flags::Terminate);
    }

    virtual void render(Engine& engine, float rd) {}

    const std::vector<float>& getTimes() const { return times; }

    virtual const char* name() const override { return "FixedTimestepTestState"; }

private:
    std::vector<float> times;
};

TEST(Engine, FixedTimestep) {
    Engine engine(Settings().withAllowVariableTimestep(false).withCreateWindow(false));

    FixedTimestepTestState* state = new FixedTimestepTestState();
    State::Ptr ptr(state);
    EXPECT_EQ(engine.run(ptr), true);

    for (unsigned int i = 0; i < state->getTimes().size() - 1; ++i) {
        EXPECT_EQ(state->getTimes().at(i), state->getTimes().at(i + 1));
    }
}

struct EventReceiver : public bl::event::Listener<Event> {
    EventReceiver(std::vector<Event>& recv)
    : recv(recv) {}

    virtual void observe(const Event& event) override {
        std::cout << "Adding event: " << event.type << std::endl;
        recv.push_back(event);
    }

private:
    std::vector<Event>& recv;
};

TEST(Engine, EventsStartShutdownStateChanges) {
    Engine engine(Settings().withCreateWindow(false));
    std::vector<Event> events;
    EventReceiver listener(events);
    engine.eventBus().subscribe(&listener);

    FlagTestState* first = new FlagTestState(Flags::PopState);
    State::Ptr firstPtr(first);
    FlagTestState* second = new FlagTestState(Flags::PopState);
    State::Ptr secondPtr(second);
    engine.nextState(secondPtr);
    ASSERT_EQ(engine.run(firstPtr), true);

    ASSERT_EQ(events.size(), 4);
    EXPECT_EQ(events[0].type, Event::Type::Startup);
    EXPECT_EQ(events[1].type, Event::Type::StateChange);
    EXPECT_EQ(events[2].type, Event::Type::StateChange);
    EXPECT_EQ(events[3].type, Event::Type::Shutdown);
    EXPECT_EQ(events[3].shutdown.cause, Event::ShutdownEvent::FinalStatePopped);
}

TEST(Engine, TerminateEvent) {
    Engine engine(Settings().withCreateWindow(false));
    std::vector<Event> events;
    EventReceiver listener(events);
    engine.eventBus().subscribe(&listener);

    FlagTestState* first = new FlagTestState(Flags::Terminate);
    State::Ptr firstPtr(first);
    ASSERT_EQ(engine.run(firstPtr), true);

    ASSERT_EQ(events.size(), 2);
    EXPECT_EQ(events[0].type, Event::Type::Startup);
    EXPECT_EQ(events[1].type, Event::Type::Shutdown);
    EXPECT_EQ(events[1].shutdown.cause, Event::ShutdownEvent::Terminated);
}

} // namespace unittest
} // namespace engine
} // namespace bl
