#include <BLIB/Events/Dispatcher.hpp>
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

} // namespace unittest
} // namespace event
} // namespace bl
