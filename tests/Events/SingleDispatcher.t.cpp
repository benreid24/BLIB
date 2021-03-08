#include <BLIB/Events/SingleDispatcher.hpp>
#include <BLIB/Events/SingleDispatcherScopeGuard.hpp>
#include <gmock/gmock.h>

namespace bl
{
namespace event
{
namespace unittest
{
struct MockListener : public SingleListener<int> {
    MOCK_METHOD(void, observe, (const int& i), (override));
};

TEST(SingleDispatcher, Dispatch) {
    MockListener listener1, listener2;
    SingleDispatcher<int> dispatcher;

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

TEST(SingleDispatcher, ScopeGuard) {
    MockListener listener1;
    MockListener listener2;
    SingleDispatcher<int> dispatcher;

    SingleDispatcherScopeGuard<int> outerGuard(dispatcher);
    outerGuard.subscribe(&listener1);

    EXPECT_CALL(listener1, observe(5));
    EXPECT_CALL(listener2, observe(5)).Times(0);
    dispatcher.dispatch(5);

    {
        SingleDispatcherScopeGuard<int> innerGuard(dispatcher);
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
} // namespace event
} // namespace bl
