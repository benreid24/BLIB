#include <BLIB/Events/Dispatcher.hpp>
#include <BLIB/Events/DispatcherScopeGuard.hpp>
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

TEST(DispatcherScopeGuard, Scoped) {
    Dispatcher dispatch;
    MockListener listener;

    EXPECT_CALL(listener, observe(5)).Times(1);
    EXPECT_CALL(listener, observe("Hello")).Times(1);

    {
        DispatcherScopeGuard guard(dispatch);
        guard.subscribe(&listener);
        dispatch.dispatch<int>(5);
        dispatch.dispatch<std::string>("Hello");
    }

    dispatch.dispatch<int>(5);
    dispatch.dispatch<std::string>("Hello");
}

TEST(DispatcherScopeGuard, Unsubscribe) {
    Dispatcher dispatch;
    DispatcherScopeGuard guard(dispatch);

    ::testing::StrictMock<MockListener> listener;
    guard.subscribe(&listener);
    guard.unsubscribe(&listener);

    dispatch.dispatch<int>(5);
    dispatch.dispatch<std::string>("Hello");
}

} // namespace unittest
} // namespace event
} // namespace bl
