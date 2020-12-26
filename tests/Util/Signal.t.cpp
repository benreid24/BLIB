#include <BLIB/Util/Signal.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace unittest
{
TEST(Signal, SetBool) {
    bool testVar1(false), testVar2(true);

    Signal s1;
    s1.willSet(testVar1, true);
    s1();

    Signal s2;
    s2.willSet(testVar2, false);
    s2();

    EXPECT_TRUE(testVar1);
    EXPECT_FALSE(testVar2);
}

TEST(Signal, CallbackCall) {
    bool cbCalled = false;
    auto cb       = [&cbCalled]() { cbCalled = true; };

    Signal s;
    s.willCall(cb);
    s();

    EXPECT_TRUE(cbCalled);
}

TEST(Signal, Clear) {
    bool set = false;

    bool cb1Called = false;
    auto cb1       = [&cb1Called]() { cb1Called = true; };

    bool cb2Called = false;
    auto cb2       = [&cb2Called]() { cb2Called = true; };

    Signal s;
    s.willSet(set, true);
    s.willCall(cb1);
    s.willAlwaysCall(cb2);
    s.clear();
    s();

    EXPECT_FALSE(set);
    EXPECT_FALSE(cb1Called);
    EXPECT_TRUE(cb2Called);
}

} // namespace unittest
} // namespace bl
