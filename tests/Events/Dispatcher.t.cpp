#include <BLIB/Events/Dispatcher.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <thread>

namespace bl
{
namespace event
{
namespace unittest
{
namespace
{
class MockListener : public Listener<int, std::string> {
public:
    virtual ~MockListener() = default;

    MOCK_METHOD(void, observe, (const int&), (override));
    MOCK_METHOD(void, observe, (const std::string&), (override));
};

class NestedListenerDispatcher : public Listener<int> {
public:
    NestedListenerDispatcher(Dispatcher& bus)
    : bus(bus) {}

    virtual ~NestedListenerDispatcher() = default;

    virtual void observe(const int&) override { bus.dispatch<char>('a'); }

private:
    Dispatcher& bus;
};

void nestedTester(bool& markTrue) {
    Dispatcher bus;

    NestedListenerDispatcher guy(bus);
    bus.subscribe(&guy);

    bus.dispatch<int>(5); // to catch deadlocks
    markTrue = true;
}

} // namespace

TEST(Dispatcher, DispatchEventsDifferentTypes) {
    Dispatcher dispatch;
    MockListener listener;
    dispatch.subscribe(&listener);

    EXPECT_CALL(listener, observe(5));
    EXPECT_CALL(listener, observe("Hello"));

    dispatch.dispatch<int>(5);
    dispatch.dispatch<std::string>("Hello");
}

TEST(Dispatcher, Unsubscribe) {
    Dispatcher dispatch;

    ::testing::StrictMock<MockListener> listener;
    dispatch.subscribe(&listener);
    dispatch.unsubscribe(&listener);

    dispatch.dispatch<int>(5);
    dispatch.dispatch<std::string>("Hello");
}

TEST(Dispatcher, NestedDispatch) {
    bool threadFinished = false;
    std::thread runner(std::bind(&nestedTester, std::ref(threadFinished)));
    runner.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    EXPECT_TRUE(threadFinished);
}

} // namespace unittest
} // namespace event
} // namespace bl
