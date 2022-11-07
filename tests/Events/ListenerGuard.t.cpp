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
    Dispatcher d;
    TestListener l;
    ListenerGuard<int> g(&l);

    g.subscribe(d);
    d.syncListeners();
    d.dispatch<int>(5);
    d.dispatch<int>(10);
    d.dispatch<int>(15);

    EXPECT_EQ(l.sum, 30);
    EXPECT_EQ(l.eventCount, 3);
}

TEST(EventClassGuard, Unsubscribe) {
    Dispatcher d;
    TestListener l;
    ListenerGuard<int> g(&l);

    g.subscribe(d);
    d.syncListeners();
    d.dispatch<int>(5);
    d.dispatch<int>(10);
    g.unsubscribe();
    d.syncListeners();
    d.dispatch<int>(15);

    EXPECT_EQ(l.sum, 15);
    EXPECT_EQ(l.eventCount, 2);
}

TEST(EventClassGuard, Destruct) {
    Dispatcher d;
    TestListener l;

    {
        ListenerGuard<int> g(&l);
        g.subscribe(d);
        d.syncListeners();
        d.dispatch<int>(5);
        d.dispatch<int>(10);
    }
    d.syncListeners();
    d.dispatch<int>(15);

    EXPECT_EQ(l.sum, 15);
    EXPECT_EQ(l.eventCount, 2);
}

} // namespace unittest
} // namespace event
} // namespace bl
