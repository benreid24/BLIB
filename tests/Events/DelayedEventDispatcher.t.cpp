#include <BLIB/Events/DelayedEventDispatcher.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace bl
{
namespace unittest
{
namespace
{
class MockListener : public MultiEventListener<int, std::string> {
public:
    virtual ~MockListener() = default;

    MOCK_METHOD(void, observe, (const int&), (override));
    MOCK_METHOD(void, observe, (const std::string&), (override));
};

} // namespace

TEST(DelayedEventDispatcher, ManualDtorDrain) {
    MultiEventDispatcher dispatch;
    MockListener listener;
    dispatch.subscribe(&listener);

    EXPECT_CALL(listener, observe(5));
    EXPECT_CALL(listener, observe("Hello"));

    {
        DelayedEventDispatcher delayed(dispatch, false);
        delayed.dispatch<int>(5);
        delayed.dispatch<std::string>("Hello");
    }
}

TEST(DelayedEventDispatcher, ManualDrain) {
    MultiEventDispatcher dispatch;
    MockListener listener;
    dispatch.subscribe(&listener);

    EXPECT_CALL(listener, observe(5));
    EXPECT_CALL(listener, observe("Hello"));

    DelayedEventDispatcher delayed(dispatch, false);
    delayed.dispatch<int>(5);
    delayed.dispatch<std::string>("Hello");
    delayed.drain();
}

TEST(DelayedEventDispatcher, BackgroundDrain) {
    MultiEventDispatcher dispatch;
    MockListener listener;
    dispatch.subscribe(&listener);

    EXPECT_CALL(listener, observe(5));
    EXPECT_CALL(listener, observe("Hello"));

    DelayedEventDispatcher delayed(dispatch, true);
    delayed.dispatch<int>(5);
    delayed.dispatch<std::string>("Hello");

    std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

} // namespace unittest
} // namespace bl
