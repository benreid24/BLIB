#include <BLIB/Util/RAIICleanup.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace util
{
namespace unittest
{
TEST(RAIICleanup, Basic) {
    int x = 0;
    {
        RAIICleanup cleanup;
        cleanup.add([&]() { x += 1; });
        cleanup.add([&]() { x += 2; });
        cleanup.add([&]() { x += 3; });
    }
    EXPECT_EQ(x, 6);
}

TEST(RAIICleanup, Disarm) {
    int x = 0;
    {
        RAIICleanup cleanup;
        cleanup.add([&]() { x += 1; });
        cleanup.add([&]() { x += 2; });
        cleanup.add([&]() { x += 3; });
        cleanup.disarm();
    }
    EXPECT_EQ(x, 0);
}

TEST(RAIICleanup, Order) {
    std::vector<int> order;
    {
        RAIICleanup cleanup;
        cleanup.add([&]() { order.push_back(1); });
        cleanup.add([&]() { order.push_back(2); });
        cleanup.add([&]() { order.push_back(3); });
    }
    EXPECT_EQ(order.size(), 3);
    EXPECT_EQ(order[0], 3);
    EXPECT_EQ(order[1], 2);
    EXPECT_EQ(order[2], 1);
}

} // namespace unittest
} // namespace util
} // namespace bl
