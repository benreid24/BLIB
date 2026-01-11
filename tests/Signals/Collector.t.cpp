#include <BLIB/Signals.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace sig
{
namespace unittest
{
namespace
{
using IntEmitter   = Emitter<int>;
using IntCollector = Collector<int>;
} // namespace

TEST(Collector, CollectAndDrain) {
    Channel channel;
    IntEmitter emitter;
    IntCollector collector;

    emitter.connect(channel);
    EXPECT_FALSE(collector.isSubscribed());
    collector.subscribe(channel);
    EXPECT_TRUE(collector.isSubscribed());

    emitter.emit(42);
    emitter.emit(100);

    std::vector<int> local;
    collector.drain([&local](const int& signal) { local.emplace_back(signal); });
    EXPECT_EQ(local.size(), 2);
    EXPECT_EQ(local[0], 42);
    EXPECT_EQ(local[1], 100);
}

TEST(Collector, DeferSubscribe) {
    Channel channel;
    IntEmitter emitter;
    IntCollector collector;

    emitter.connect(channel);
    EXPECT_FALSE(collector.isSubscribed());
    collector.subscribeDeferred(channel);

    emitter.emit(42);
    channel.syncDeferred();
    emitter.emit(100);

    std::vector<int> local;
    collector.drain([&local](const int& signal) { local.emplace_back(signal); });
    EXPECT_EQ(local.size(), 1);
    EXPECT_EQ(local[0], 100);
}

TEST(Collector, Unsubscribe) {
    Channel channel;
    IntEmitter emitter;
    IntCollector collector;

    emitter.connect(channel);
    EXPECT_FALSE(collector.isSubscribed());
    collector.subscribe(channel);
    EXPECT_TRUE(collector.isSubscribed());

    emitter.emit(42);

    collector.unsubscribe();
    EXPECT_FALSE(collector.isSubscribed());

    emitter.emit(100);

    std::vector<int> local;
    collector.drain([&local](const int& signal) { local.emplace_back(signal); });
    EXPECT_EQ(local.size(), 1);
    EXPECT_EQ(local[0], 42);
}

} // namespace unittest
} // namespace sig
} // namespace bl
