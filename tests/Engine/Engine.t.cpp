#include <BLIB/Engine.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Logging.hpp>
#include <any>
#include <gtest/gtest.h>
#include <vector>

namespace bl
{
namespace engine
{
namespace unittest
{
namespace
{
class FlagTestState : public State {
public:
    FlagTestState(Flags::Flag flag, State::Ptr next = {}, bool replace = false)
    : callCount(0)
    , n(next)
    , r(replace)
    , flag(flag) {}

    virtual void update(Engine& engine, float dt) {
        if (n) {
            if (r)
                engine.replaceState(n);
            else
                engine.pushState(n);
            n.reset();
        }
        else {
            engine.flags().set(flag);
        }
        ++callCount;
    }

    virtual void render(Engine& engine, float rd) {}

    int timesUpdated() const { return callCount; }

    virtual const char* name() const override { return "FlagTestState"; }

private:
    const Flags::Flag flag;
    State::Ptr n;
    bool r;
    int callCount;
};

} // namespace

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

TEST(Engine, ReplaceState) {
    Engine engine(Settings().withCreateWindow(false));

    FlagTestState* second = new FlagTestState(Flags::PopState);
    State::Ptr secondPtr(second);
    FlagTestState* first = new FlagTestState(Flags::PopState, secondPtr, true);
    State::Ptr firstPtr(first);

    // update1(s) -> pop -> next state -> update2(s) -> pop -> end
    EXPECT_EQ(engine.run(firstPtr), true);
    EXPECT_GE(first->timesUpdated(), 1);
    EXPECT_GE(second->timesUpdated(), 1);
}

TEST(Engine, PushState) {
    Engine engine(Settings().withCreateWindow(false));

    FlagTestState* second = new FlagTestState(Flags::PopState);
    State::Ptr secondPtr(second);
    FlagTestState* first = new FlagTestState(Flags::PopState, secondPtr, false);
    State::Ptr firstPtr(first);

    // update1(s) -> push -> next state -> update2(s) -> pop -> update1(s) -> end
    engine.pushState(secondPtr);
    EXPECT_EQ(engine.run(firstPtr), true);
    EXPECT_GE(first->timesUpdated(), 2);
    EXPECT_GE(second->timesUpdated(), 1);
}

namespace
{
class VariableTimeTestState : public State {
public:
    VariableTimeTestState()
    : state(Constant)
    , counter(0) {
        levelTimes.reserve(20);
        shortTimes.reserve(20);
        longTimes.reserve(20);
    }

    virtual const char* name() const override { return "VariableTimeTestState"; }

    virtual void update(Engine& engine, float dt) {
        BL_LOG_INFO << "update() called with timestep: " << dt << "s";
        switch (state) {
        case Constant:
            levelTimes.push_back(dt);
            if (counter >= 20) {
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
            longTimes.push_back(dt);
            if (counter >= 20) {
                counter = 0;
                state   = Decreasing;
                sf::sleep(sf::seconds(dt / 3.f));
            }
            else {
                ++counter;
                sf::sleep(sf::seconds(dt + 0.1f));
            }
            break;

        case Decreasing:
            shortTimes.push_back(dt);
            if (counter >= 20) { engine.flags().set(Flags::Terminate); }
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

    const float levelAvg() {
        float s = 0;
        for (const float t : levelTimes) { s += t; }
        return s / static_cast<float>(levelTimes.size());
    }

    const float longAvg() {
        float s = 0;
        for (const float t : longTimes) { s += t; }
        return s / static_cast<float>(longTimes.size());
    }

    const float shortAvg() {
        float s = 0;
        for (const float t : shortTimes) { s += t; }
        return s / static_cast<float>(shortTimes.size());
    }

private:
    std::vector<float> levelTimes;
    std::vector<float> longTimes;
    std::vector<float> shortTimes;
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
    EXPECT_FLOAT_EQ(state->levelAvg(), 0.1f);
    EXPECT_GT(state->longAvg(), state->levelAvg());
    EXPECT_LT(state->shortAvg(), state->longAvg());
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

} // namespace

TEST(Engine, FixedTimestep) {
    Engine engine(Settings().withAllowVariableTimestep(false).withCreateWindow(false));

    FixedTimestepTestState* state = new FixedTimestepTestState();
    State::Ptr ptr(state);
    EXPECT_EQ(engine.run(ptr), true);

    for (unsigned int i = 0; i < state->getTimes().size() - 1; ++i) {
        EXPECT_EQ(state->getTimes().at(i), state->getTimes().at(i + 1));
    }
}

namespace
{
struct EventReceiver
: public bl::event::Listener<event::Startup, event::StateChange, event::Shutdown> {
    EventReceiver(std::vector<std::any>& recv)
    : recv(recv) {}

    virtual void observe(const event::Startup& event) override { recv.push_back({event}); }
    virtual void observe(const event::StateChange& event) override { recv.push_back({event}); }
    virtual void observe(const event::Shutdown& event) override { recv.push_back({event}); }

private:
    std::vector<std::any>& recv;
};

} // namespace

TEST(Engine, EventsStartShutdownStateChanges) {
    Engine engine(Settings().withCreateWindow(false));
    std::vector<std::any> events;
    EventReceiver listener(events);
    engine.eventBus().subscribe(&listener);

    FlagTestState* first = new FlagTestState(Flags::PopState);
    State::Ptr firstPtr(first);
    FlagTestState* second = new FlagTestState(Flags::PopState);
    State::Ptr secondPtr(second);
    engine.pushState(secondPtr);
    ASSERT_EQ(engine.run(firstPtr), true);

    ASSERT_EQ(events.size(), 4);
    EXPECT_EQ(events[0].type(), typeid(event::Startup));
    EXPECT_EQ(events[1].type(), typeid(event::StateChange));
    EXPECT_EQ(events[2].type(), typeid(event::StateChange));
    EXPECT_EQ(events[3].type(), typeid(event::Shutdown));
    EXPECT_EQ(std::any_cast<event::Shutdown>(events[3]).cause, event::Shutdown::FinalStatePopped);
}

TEST(Engine, TerminateEvent) {
    Engine engine(Settings().withCreateWindow(false));
    std::vector<std::any> events;
    EventReceiver listener(events);
    engine.eventBus().subscribe(&listener);

    FlagTestState* first = new FlagTestState(Flags::Terminate);
    State::Ptr firstPtr(first);
    ASSERT_EQ(engine.run(firstPtr), true);

    ASSERT_EQ(events.size(), 2);
    EXPECT_EQ(events[0].type(), typeid(event::Startup));
    EXPECT_EQ(events[1].type(), typeid(event::Shutdown));
    EXPECT_EQ(std::any_cast<event::Shutdown>(events[1]).cause, event::Shutdown::Terminated);
}

namespace
{
class TimeTestState : public bl::engine::State {
public:
    TimeTestState()
    : totalTime(0) {}

    virtual void update(Engine& engine, float dt) {
        totalTime += dt;
        if (totalTime >= 5.f) { engine.flags().set(Flags::Terminate); }
    }

    virtual void render(Engine& engine, float rd) {}

    float timeElapsed() const { return totalTime; }

    virtual const char* name() const override { return "TimeTestState"; }

private:
    float totalTime;
};

} // namespace

TEST(Engine, TimeElapsedParity) {
    Engine engine(Settings().withCreateWindow(false));
    TimeTestState* state = new TimeTestState();
    State::Ptr ptr(state);

    ASSERT_EQ(engine.run(ptr), 1);
    EXPECT_GE(state->timeElapsed(), 5.f);
    EXPECT_LE(state->timeElapsed(), 5.2f);
}

} // namespace unittest
} // namespace engine
} // namespace bl
