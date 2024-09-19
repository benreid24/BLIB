#include <BLIB/Util/VariadicHelpers.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace util
{
namespace unittest
{
TEST(Util, VariadicHelpers) {
    const bool valueContained    = VariadicListContains<int, float, char, short, int>::value;
    const bool valueNotContained = VariadicListContains<int, float, char, short, int>::value;
    EXPECT_TRUE(valueContained);
    EXPECT_FALSE(valueNotContained);

    const bool setContained =
        VariadicSetsContained<Variadic<int, char>,
                              Variadic<short, float, int, unsigned, char>>::value;
    const bool setNotContained =
        VariadicSetsContained<Variadic<int, char>, Variadic<short, float, unsigned, char>>::value;
    EXPECT_TRUE(setContained);
    EXPECT_FALSE(setNotContained);
}

} // namespace unittest
} // namespace util
} // namespace bl
