#include <BLIB/Util/TaskScheduler.hpp>
#include <SFML/System.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace util
{
namespace unittest
{
TEST(TaskScheduler, ImmediateSingle) {
    TaskScheduler scheduler;

    int count = 0;
    auto task = [&count]() { ++count; };

    scheduler.scheduleTask(task, 1.f);
    scheduler.update(0.5f);
    EXPECT_EQ(count, 0);
    scheduler.update(0.6f);
    EXPECT_EQ(count, 1);
    scheduler.update(1.1f);
    EXPECT_EQ(count, 1);
}

TEST(TaskScheduler, ImmediateInterval) {
    TaskScheduler scheduler;

    int count = 0;
    auto task = [&count]() { ++count; };

    scheduler.scheduleRepeatedTask(task, 1.f, 0.5f);
    scheduler.update(0.6f);
    EXPECT_EQ(count, 1);
    scheduler.update(1.f);
    EXPECT_EQ(count, 2);
}

TEST(TaskScheduler, ThreadPoolSingle) {
    ThreadPool pool;
    TaskScheduler scheduler(pool);
    pool.start(1);

    int count = 0;
    auto task = [&count]() { ++count; };

    scheduler.scheduleTask(task, 1.f);
    scheduler.update(0.5f);
    sf::sleep(sf::milliseconds(20));
    EXPECT_EQ(count, 0);
    scheduler.update(0.6f);
    sf::sleep(sf::milliseconds(20));
    EXPECT_EQ(count, 1);
    scheduler.update(1.1f);
    sf::sleep(sf::milliseconds(20));
    EXPECT_EQ(count, 1);

    pool.shutdown();
}

TEST(TaskScheduler, ThreadPoolInterval) {
    ThreadPool pool;
    TaskScheduler scheduler(pool);
    pool.start(1);

    int count = 0;
    auto task = [&count]() { ++count; };

    scheduler.scheduleRepeatedTask(task, 1.f, 0.5f);
    scheduler.update(0.6f);
    sf::sleep(sf::milliseconds(20));
    EXPECT_EQ(count, 1);
    scheduler.update(1.f);
    sf::sleep(sf::milliseconds(20));
    EXPECT_EQ(count, 2);

    pool.shutdown();
}

TEST(TaskScheduler, Cancel) {
    TaskScheduler scheduler;

    int count = 0;
    auto task = [&count]() { ++count; };

    auto singleId      = scheduler.scheduleTask(task, 1.f);
    auto singleIdShort = scheduler.scheduleTask(task, 0.5f);
    auto intervalId    = scheduler.scheduleRepeatedTask(task, 0.5f);

    scheduler.update(0.6f);
    EXPECT_EQ(count, 2);                               // short + interval
    EXPECT_FALSE(scheduler.cancelTask(singleIdShort)); // already ran
    EXPECT_TRUE(scheduler.cancelTask(singleId));       // not yet ran

    scheduler.update(0.6f);
    EXPECT_EQ(count, 3); // interval again
    EXPECT_TRUE(scheduler.cancelTask(intervalId));

    scheduler.update(0.6f);
    EXPECT_EQ(count, 3);
}

} // namespace unittest
} // namespace util
} // namespace bl
