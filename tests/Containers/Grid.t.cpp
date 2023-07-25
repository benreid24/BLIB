#include <BLIB/Containers/Grid.hpp>
#include <gtest/gtest.h>
#include <unordered_set>

namespace bl
{
namespace ctr
{
namespace unittest
{
TEST(Grid, Add) {
    Grid<int> grid({0.f, 0.f, 100.f, 100.f}, 10.f, 10.f);

    grid.add({55.f, 55.f}, 15);
    const auto& cell = grid.getCell({55.f, 55.f});
    ASSERT_EQ(cell.size(), 1);
    EXPECT_EQ(cell[0], 15);
}

TEST(Grid, Move) {
    Grid<int> grid({0.f, 0.f, 100.f, 100.f}, 10.f, 10.f);

    grid.add({55.f, 55.f}, 15);
    const auto& cell = grid.getCell({55.f, 55.f});
    ASSERT_EQ(cell.size(), 1);
    EXPECT_EQ(cell[0], 15);

    grid.move({55.f, 55.f}, {25.f, 25.f}, 15);
    EXPECT_TRUE(cell.empty());

    auto& newCell = grid.getCell({25.f, 25.f});
    ASSERT_EQ(newCell.size(), 1);
    EXPECT_EQ(newCell[0], 15);
}

TEST(Grid, Remove) {
    Grid<int> grid({0.f, 0.f, 100.f, 100.f}, 10.f, 10.f);

    grid.add({55.f, 55.f}, 15);
    const auto& cell = grid.getCell({55.f, 55.f});
    ASSERT_EQ(cell.size(), 1);
    EXPECT_EQ(cell[0], 15);

    grid.remove({55.f, 55.f}, 15);
    EXPECT_TRUE(cell.empty());
}

TEST(Grid, Clear) {
    Grid<int> grid({0.f, 0.f, 100.f, 100.f}, 10.f, 10.f);

    grid.add({55.f, 55.f}, 15);
    const auto& cell = grid.getCell({55.f, 55.f});
    ASSERT_EQ(cell.size(), 1);
    EXPECT_EQ(cell[0], 15);

    grid.clear();
    EXPECT_TRUE(cell.empty());
}

TEST(Grid, IterateRegion) {
    std::unordered_set<int> inRegion({15, 10, 30});
    Grid<int> grid({0.f, 0.f, 100.f, 100.f}, 10.f, 10.f);

    // in region
    grid.add({35.f, 35.f}, 15);
    grid.add({20.f, 25.f}, 10);
    grid.add({11.f, 39.f}, 30);

    // outside of region
    grid.add({41.f, 25.f}, 11);
    grid.add({5.f, 25.f}, 45);
    grid.add({20.f, 75.f}, 76);

    const auto visitor = [&inRegion](int val) {
        const auto it = inRegion.find(val);
        ASSERT_NE(it, inRegion.end()) << "Failed to find " + std::to_string(val);
        inRegion.erase(it);
    };
    grid.forAllInRegion({10.f, 10.f, 29.f, 29.f}, visitor);
    EXPECT_TRUE(inRegion.empty());
}

TEST(Grid, IterateCells) {
    std::unordered_set<int> inRegion({15, 10, 30});
    Grid<int> grid({0.f, 0.f, 100.f, 100.f}, 10.f, 10.f);

    // in region
    grid.add({45.f, 45.f}, 15);
    grid.add({55.f, 55.f}, 10);
    grid.add({65.f, 65.f}, 30);

    // outside of region
    grid.add({41.f, 25.f}, 11);
    grid.add({5.f, 25.f}, 45);
    grid.add({20.f, 75.f}, 76);

    const auto visitor = [&inRegion](int val) {
        const auto it = inRegion.find(val);
        ASSERT_NE(it, inRegion.end()) << "Failed to find " + std::to_string(val);
        inRegion.erase(it);
    };
    grid.forAllInCellAndNeighbors({55.f, 55.f}, visitor);
    EXPECT_TRUE(inRegion.empty());
}

TEST(Grid, IterateEndEarly) {
    Grid<int> grid({0.f, 0.f, 100.f, 100.f}, 10.f, 10.f);

    // in region
    grid.add({45.f, 45.f}, 15);
    grid.add({55.f, 55.f}, 10);
    grid.add({65.f, 65.f}, 30);

    // outside of region
    grid.add({41.f, 25.f}, 11);
    grid.add({5.f, 25.f}, 45);
    grid.add({20.f, 75.f}, 76);

    int lastSeen = 0;

    const auto visitor = [&lastSeen](int val) {
        lastSeen = val;
        return val == 10;
    };

    grid.forAllInCellAndNeighbors({55.f, 55.f}, visitor);
    EXPECT_EQ(lastSeen, 10);
}

TEST(Grid, Pointer) {
    std::vector<std::vector<long*>> tvec(32);

    Grid<int*> grid({0.f, 0.f, 100.f, 100.f}, 10.f, 10.f);

    int val = 55;
    grid.add({55.f, 55.f}, &val);
    auto& cell = grid.getCell({55.f, 55.f});
    ASSERT_EQ(cell.size(), 1);
    EXPECT_EQ(*cell.front(), val);
}

} // namespace unittest
} // namespace ctr
} // namespace bl
