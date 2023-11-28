#include <BLIB/Util/ThreadPool.hpp>
#include <array>
#include <gtest/gtest.h>

namespace bl
{
namespace util
{
namespace unittest
{
TEST(ThreadPool, StartupAndShutdown) {
    const auto test = []() {
        ThreadPool pool;
        EXPECT_FALSE(pool.running());
        pool.start();
        EXPECT_TRUE(pool.running());
        pool.shutdown();
        EXPECT_FALSE(pool.running());
    };

    auto future       = std::async(test);
    const auto result = future.wait_for(std::chrono::milliseconds(750));
    EXPECT_NE(result, std::future_status::timeout);
    if (result == std::future_status::timeout) { std::terminate(); }
}

TEST(ThreadPool, ExecuteTasks) {
    ThreadPool pool;
    std::array<bool, 16> flags;
    flags.fill(false);

    auto notQueued = pool.queueTask([]() {});
    EXPECT_FALSE(notQueued.valid());

    pool.start(4);
    for (unsigned int i = 0; i < 16; ++i) {
        pool.queueTask([&flags, i]() { flags[i] = true; });
    }
    pool.shutdown();
    for (unsigned int i = 0; i < 16; ++i) { EXPECT_TRUE(flags[i]); }

    // verify restart
    pool.start(2);
    for (unsigned int i = 0; i < 16; ++i) {
        pool.queueTask([&flags, i]() { flags[i] = false; });
    }
    pool.shutdown();
    for (unsigned int i = 0; i < 16; ++i) { EXPECT_FALSE(flags[i]); }
}

TEST(ThreadPool, ShutdownLongRunningTask) {
    const auto longTask = []() { std::this_thread::sleep_for(std::chrono::milliseconds(300)); };

    const auto test = [&longTask]() {
        ThreadPool pool;
        EXPECT_FALSE(pool.running());
        pool.start(2);
        pool.queueTask(longTask);
        pool.queueTask(longTask);
        EXPECT_TRUE(pool.running());
        pool.shutdown();
        EXPECT_FALSE(pool.running());
    };

    auto future       = std::async(test);
    const auto result = future.wait_for(std::chrono::milliseconds(1000));
    EXPECT_NE(result, std::future_status::timeout);
    if (result == std::future_status::timeout) { std::terminate(); }
}

TEST(ThreadPool, QueueWhileDraining) {
    bool parentRan = false;
    bool childRan  = false;

    ThreadPool pool;
    const auto child  = [&childRan]() { childRan = true; };
    const auto parent = [&pool, &parentRan, &child]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        pool.queueTask(child);
        parentRan = true;
    };

    const auto test = [&parent, &pool, &childRan, &parentRan]() {
        pool.queueTask(parent);
        pool.drain();
        EXPECT_TRUE(childRan);
        EXPECT_TRUE(parentRan);
    };

    pool.start(2);
    auto future       = std::async(test);
    const auto result = future.wait_for(std::chrono::milliseconds(750));
    EXPECT_NE(result, std::future_status::timeout);
    if (result == std::future_status::timeout) { std::terminate(); }
}

TEST(ThreadPool, ShutdownStoppedPool) {
    ThreadPool pool;
    pool.shutdown();
}

} // namespace unittest
} // namespace util
} // namespace bl
