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
    virtual ~NestedListenerDispatcher() = default;

    virtual void observe(const int&) override { bl::event::Dispatcher::dispatch<char>('a'); }
};

void nestedTester(bool& markTrue) {
    NestedListenerDispatcher guy;
    bl::event::Dispatcher::subscribe(&guy);

    bl::event::Dispatcher::dispatch<int>(5); // to catch deadlocks
    markTrue = true;

    // cleanup
    bl::event::Dispatcher::clearAllListeners();
}

} // namespace

TEST(Dispatcher, DispatchEventsDifferentTypes) {
    MockListener listener;
    bl::event::Dispatcher::subscribe(&listener);
    bl::event::Dispatcher::syncListeners();

    EXPECT_CALL(listener, observe(5));
    EXPECT_CALL(listener, observe("Hello"));

    bl::event::Dispatcher::dispatch<int>(5);
    bl::event::Dispatcher::dispatch<std::string>("Hello");

    // cleanup
    bl::event::Dispatcher::clearAllListeners();
}

TEST(Dispatcher, Unsubscribe) {
    ::testing::StrictMock<MockListener> listener;
    bl::event::Dispatcher::subscribe(&listener);
    bl::event::Dispatcher::unsubscribe(&listener);

    bl::event::Dispatcher::dispatch<int>(5);
    bl::event::Dispatcher::dispatch<std::string>("Hello");

    // cleanup
    bl::event::Dispatcher::clearAllListeners();
}

TEST(Dispatcher, NestedDispatch) {
    bool threadFinished = false;
    std::thread runner(std::bind(&nestedTester, std::ref(threadFinished)));
    runner.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    EXPECT_TRUE(threadFinished);

    // cleanup
    bl::event::Dispatcher::clearAllListeners();
}

} // namespace unittest
} // namespace event
} // namespace bl
