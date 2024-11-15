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
    : State(StateMask::All)
    , callCount(0)
    , n(next)
    , r(replace)
    , flag(flag) {}

    virtual void update(Engine& engine, float, float) override {
        if (n) {
            if (r)
                engine.replaceState(n);
            else
                engine.pushState(n);
            n.reset();
        }
        else { engine.flags().set(flag); }
        ++callCount;
    }

    int timesUpdated() const { return callCount; }

    virtual const char* name() const override { return "FlagTestState"; }

    virtual void activate(Engine&) override {}
    virtual void deactivate(Engine&) override {}

private:
    int callCount;
    State::Ptr n;
    bool r;
    const Flags::Flag flag;
};

} // namespace

TEST(Engine, Terminate) {
    Engine engine(Settings{});

    FlagTestState* state = new FlagTestState(Flags::Terminate);
    State::Ptr ptr(state);
    EXPECT_EQ(engine.run(ptr), true);
    EXPECT_EQ(state->timesUpdated(), 1);
    EXPECT_EQ(ptr.use_count(), 2);
}

TEST(Engine, PopState) {
    Engine engine(Settings{});

    FlagTestState* state = new FlagTestState(Flags::PopState);
    State::Ptr ptr(state);
    EXPECT_EQ(engine.run(ptr), true);
    EXPECT_EQ(state->timesUpdated(), 1);
    EXPECT_EQ(ptr.use_count(), 1);
}

TEST(Engine, ReplaceState) {
    Engine engine(Settings{});

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
    Engine engine(Settings{});

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
class FixedTimestepTestState : public State {
public:
    FixedTimestepTestState()
    : State(StateMask::All) {}

    virtual void update(Engine& engine, float dt, float) override {
        times.push_back(dt);
        if (times.size() >= 10) engine.flags().set(Flags::Terminate);
    }

    const std::vector<float>& getTimes() const { return times; }

    virtual const char* name() const override { return "FixedTimestepTestState"; }

    virtual void activate(Engine&) override {}
    virtual void deactivate(Engine&) override {}

private:
    std::vector<float> times;
};

} // namespace

TEST(Engine, FixedTimestep) {
    Engine engine(Settings{}.withAllowVariableTimestep(false));

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
    Engine engine(Settings{});
    std::vector<std::any> events;
    EventReceiver listener(events);
    bl::event::Dispatcher::subscribe(&listener);

    FlagTestState* second = new FlagTestState(Flags::PopState);
    State::Ptr secondPtr(second);
    FlagTestState* first = new FlagTestState(Flags::PopState, secondPtr, true);
    State::Ptr firstPtr(first);
    ASSERT_EQ(engine.run(firstPtr), true);

    ASSERT_EQ(events.size(), 3);
    EXPECT_EQ(events[0].type(), typeid(event::Startup));
    EXPECT_EQ(events[1].type(), typeid(event::StateChange));
    EXPECT_EQ(events[2].type(), typeid(event::Shutdown));
    EXPECT_EQ(std::any_cast<event::Shutdown>(events[2]).cause, event::Shutdown::FinalStatePopped);

    bl::event::Dispatcher::clearAllListeners();
}

TEST(Engine, TerminateEvent) {
    Engine engine(Settings{});
    std::vector<std::any> events;
    EventReceiver listener(events);
    bl::event::Dispatcher::subscribe(&listener);

    FlagTestState* first = new FlagTestState(Flags::Terminate);
    State::Ptr firstPtr(first);
    ASSERT_EQ(engine.run(firstPtr), true);

    ASSERT_EQ(events.size(), 2);
    EXPECT_EQ(events[0].type(), typeid(event::Startup));
    EXPECT_EQ(events[1].type(), typeid(event::Shutdown));
    EXPECT_EQ(std::any_cast<event::Shutdown>(events[1]).cause, event::Shutdown::Terminated);

    bl::event::Dispatcher::clearAllListeners();
}

namespace
{
class TimeTestState : public bl::engine::State {
public:
    TimeTestState()
    : State(StateMask::All)
    , totalTime(0.f)
    , simulatedTime(0.f) {}

    virtual void update(Engine& engine, float dt, float realDt) override {
        totalTime += realDt;
        simulatedTime += dt;
        if (totalTime >= 5.f) { engine.flags().set(Flags::Terminate); }
    }

    float timeElapsed() const { return totalTime; }

    float timeSimulated() const { return simulatedTime; }

    virtual const char* name() const override { return "TimeTestState"; }

    virtual void activate(Engine&) override {}
    virtual void deactivate(Engine&) override {}

private:
    float totalTime;
    float simulatedTime;
};

} // namespace

TEST(Engine, TimeElapsedParity) {
    Engine engine(Settings{});
    TimeTestState* state = new TimeTestState();
    State::Ptr ptr(state);

    engine.setTimeScale(2.f);
    ASSERT_EQ(engine.run(ptr), 1);
    EXPECT_GE(state->timeElapsed(), 5.f);
    EXPECT_LE(state->timeElapsed(), 5.2f);
    EXPECT_GE(state->timeSimulated(), 10.f);
    EXPECT_LE(state->timeSimulated(), 10.2f);
}

} // namespace unittest
} // namespace engine
} // namespace bl
