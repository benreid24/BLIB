#include <BLIB/Events/DelayedDispatcher.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

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

} // namespace

TEST(DelayedDispatcher, ManualDtorDrain) {
    Dispatcher dispatch;
    MockListener listener;
    dispatch.subscribe(&listener);

    EXPECT_CALL(listener, observe(5));
    EXPECT_CALL(listener, observe("Hello"));

    {
        DelayedDispatcher delayed(dispatch, false);
        delayed.dispatch<int>(5);
        delayed.dispatch<std::string>("Hello");
    }
}

TEST(DelayedDispatcher, ManualDrain) {
    Dispatcher dispatch;
    MockListener listener;
    dispatch.subscribe(&listener);

    EXPECT_CALL(listener, observe(5));
    EXPECT_CALL(listener, observe("Hello"));

    DelayedDispatcher delayed(dispatch, false);
    delayed.dispatch<int>(5);
    delayed.dispatch<std::string>("Hello");
    delayed.drain();
}

TEST(DelayedDispatcher, BackgroundDrain) {
    Dispatcher dispatch;
    MockListener listener;
    dispatch.subscribe(&listener);

    EXPECT_CALL(listener, observe(5));
    EXPECT_CALL(listener, observe("Hello"));

    DelayedDispatcher delayed(dispatch, true);
    delayed.dispatch<int>(5);
    delayed.dispatch<std::string>("Hello");

    std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

} // namespace unittest
} // namespace event
} // namespace bl
