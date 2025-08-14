#include <BLIB/Signals.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace sig
{
namespace unittest
{
TEST(Router, HappyPath) {
    Channel channel;
    Emitter<int, float> emitter;
    Router<int, float> router;

    emitter.connect(channel);
    router.subscribe(channel);

    int intSignal     = 42;
    float floatSignal = 3.14f;
    bool intCalled    = false;
    bool floatCalled  = false;
    router.route<int>([&intCalled](const int& signal) {
        EXPECT_EQ(signal, 42);
        intCalled = true;
    });
    router.route<float>([&floatCalled](const float& signal) {
        EXPECT_FLOAT_EQ(signal, 3.14f);
        floatCalled = true;
    });
    emitter.emit<int>(intSignal);
    emitter.emit<float>(floatSignal);
    EXPECT_TRUE(intCalled);
    EXPECT_TRUE(floatCalled);
}

} // namespace unittest
} // namespace sig
} // namespace bl
