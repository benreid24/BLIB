#include <BLIB/Events/EventDispatcher.hpp>
#include <BLIB/Events/EventDispatcherScopeGuard.hpp>
#include <gmock/gmock.h>

namespace bl
{
namespace unittest
{
struct MockListener : public EventListener<int> {
    MOCK_METHOD(void, observe, (const int& i), (override));
};

TEST(EventDispatcher, Dispatch) {
    MockListener listener1, listener2;
    EventDispatcher<int> dispatcher;

    dispatcher.subscribe(&listener1);
    dispatcher.subscribe(&listener2);

    EXPECT_CALL(listener1, observe(5));
    EXPECT_CALL(listener2, observe(5));
    dispatcher.dispatch(5);

    dispatcher.remove(&listener1);
    EXPECT_CALL(listener1, observe(-1)).Times(0);
    EXPECT_CALL(listener2, observe(-1));
    dispatcher.dispatch(-1);
}

TEST(EventDispatcher, ScopeGuard) {
    MockListener listener1;
    MockListener listener2;
    EventDispatcher<int> dispatcher;

    EventDispatcherScopeGuard<int> outerGuard(dispatcher);
    outerGuard.subscribe(&listener1);

    EXPECT_CALL(listener1, observe(5));
    EXPECT_CALL(listener2, observe(5)).Times(0);
    dispatcher.dispatch(5);

    {
        EventDispatcherScopeGuard<int> innerGuard(dispatcher);
        innerGuard.subscribe(&listener2);

        EXPECT_CALL(listener1, observe(5));
        EXPECT_CALL(listener2, observe(5));
        dispatcher.dispatch(5);
    }

    EXPECT_CALL(listener1, observe(5));
    EXPECT_CALL(listener2, observe(5)).Times(0);
    dispatcher.dispatch(5);
}

} // namespace unittest
} // namespace bl
