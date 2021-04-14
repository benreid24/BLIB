#include <BLIB/Containers/Grid.hpp>
#include <gtest/gtest.h>
#include <unordered_set>

namespace bl
{
namespace container
{
namespace unittest
{
TEST(Grid, IterateAll) {
    Grid<int> grid(100, 100, 10, 10);

    grid.add(5, 5, 52);
    grid.add(95, 64, 23);
    grid.add(99, 23, 87);

    std::unordered_set<int> found;
    auto range = grid.getArea(0, 0, 100, 100);
    for (const Grid<int>::Payload& i : range) { found.insert(i.get()); }

    EXPECT_NE(found.find(52), found.end());
    EXPECT_NE(found.find(23), found.end());
    EXPECT_NE(found.find(87), found.end());
}

} // namespace unittest
} // namespace container
} // namespace bl
