#include <BLIB/Util/Waiter.hpp>
#include <gtest/gtest.h>
#include <thread>

namespace bl
{
namespace util
{
namespace unittest
{
TEST(Waiter, Wait) {
    Waiter waiter;
    bool ran = false;

    std::thread thread([&waiter, &ran]() {
        waiter.wait();
        ran = true;
    });

    EXPECT_FALSE(ran);
    waiter.unblock();
    thread.join();
    EXPECT_TRUE(ran);
}

TEST(Waiter, Immediate) {
    Waiter waiter;
    bool ran = false;

    waiter.unblock();
    std::thread thread([&waiter, &ran]() {
        waiter.wait();
        ran = true;
    });
    thread.join();
    EXPECT_TRUE(ran);
}

TEST(Waiter, Reset) {
    Waiter waiter;
    bool ran = false;

    waiter.unblock();
    waiter.reset();
    std::thread thread([&waiter, &ran]() {
        waiter.wait();
        ran = true;
    });

    EXPECT_FALSE(ran);
    waiter.unblock();
    thread.join();
    EXPECT_TRUE(ran);
}

} // namespace unittest
} // namespace util
} // namespace bl
