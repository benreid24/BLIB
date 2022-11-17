#include <BLIB/Events.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace event
{
namespace unittest
{
namespace
{
struct TestListener : public Listener<int> {
    TestListener()
    : eventCount(0)
    , sum(0) {}

    virtual void observe(const int& e) override {
        ++eventCount;
        sum += e;
    }

    int eventCount;
    int sum;
};
} // namespace

TEST(EventClassGuard, Subscribe) {
    TestListener l;
    ListenerGuard<int> g(&l);

    g.subscribe(true);
    bl::event::Dispatcher::syncListeners();
    bl::event::Dispatcher::dispatch<int>(5);
    bl::event::Dispatcher::dispatch<int>(10);
    bl::event::Dispatcher::dispatch<int>(15);

    EXPECT_EQ(l.sum, 30);
    EXPECT_EQ(l.eventCount, 3);

    // cleanup
    bl::event::Dispatcher::clearAllListeners();
}

TEST(EventClassGuard, Unsubscribe) {
    TestListener l;
    ListenerGuard<int> g(&l);

    g.subscribe();
    bl::event::Dispatcher::dispatch<int>(5);
    bl::event::Dispatcher::dispatch<int>(10);
    g.unsubscribe();
    bl::event::Dispatcher::dispatch<int>(15);

    EXPECT_EQ(l.sum, 15);
    EXPECT_EQ(l.eventCount, 2);

    // cleanup
    bl::event::Dispatcher::clearAllListeners();
}

TEST(EventClassGuard, Destruct) {
    TestListener l;

    {
        ListenerGuard<int> g(&l);
        g.subscribe();
        bl::event::Dispatcher::dispatch<int>(5);
        bl::event::Dispatcher::dispatch<int>(10);
    }
    bl::event::Dispatcher::dispatch<int>(15);

    EXPECT_EQ(l.sum, 15);
    EXPECT_EQ(l.eventCount, 2);

    // cleanup
    bl::event::Dispatcher::clearAllListeners();
}

} // namespace unittest
} // namespace event
} // namespace bl
