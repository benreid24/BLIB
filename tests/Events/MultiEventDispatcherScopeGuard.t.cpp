#include <BLIB/Events/MultiEventDispatcher.hpp>
#include <BLIB/Events/MultiEventDispatcherScopeGuard.hpp>
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

TEST(MultiEventDispatcherScopeGuard, Scoped) {
    MultiEventDispatcher dispatch;
    MockListener listener;

    EXPECT_CALL(listener, observe(5)).Times(1);
    EXPECT_CALL(listener, observe("Hello")).Times(1);

    {
        MultiEventDispatcherScopeGuard guard(dispatch);
        guard.subscribe(&listener);
        dispatch.dispatch<int>(5);
        dispatch.dispatch<std::string>("Hello");
    }

    dispatch.dispatch<int>(5);
    dispatch.dispatch<std::string>("Hello");
}

TEST(MultiEventDispatcherScopeGuard, Unsubscribe) {
    MultiEventDispatcher dispatch;
    MultiEventDispatcherScopeGuard guard(dispatch);

    ::testing::StrictMock<MockListener> listener;
    guard.subscribe(&listener);
    guard.unsubscribe(&listener);

    dispatch.dispatch<int>(5);
    dispatch.dispatch<std::string>("Hello");
}

} // namespace unittest
} // namespace bl
