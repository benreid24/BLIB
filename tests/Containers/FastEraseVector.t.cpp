#include <BLIB/Containers/FastEraseVector.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace container
{
namespace unittest
{
TEST(FastEraseVector, Erase) {
    FastEraseVector<int> vec;

    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);
    vec.erase(0);
    vec.erase(vec.begin() + 2);

    EXPECT_EQ(vec[0], 5);
    EXPECT_EQ(vec[1], 2);
    EXPECT_EQ(vec[2], 4);
}

} // namespace unittest
} // namespace container
} // namespace bl
