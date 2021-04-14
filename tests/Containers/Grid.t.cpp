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
    auto range = grid.getAll();
    for (const Grid<int>::Payload& i : range) { found.insert(i.get()); }

    EXPECT_NE(found.find(52), found.end());
    EXPECT_NE(found.find(23), found.end());
    EXPECT_NE(found.find(87), found.end());
}

TEST(Grid, IterateArea) {
    Grid<int> grid(100, 100, 10, 10);

    grid.add(5, 5, 52);
    grid.add(95, 64, 23);
    grid.emplace(99, 23, 87);

    std::unordered_set<int> found;
    auto range = grid.getArea(90, 20, 12, 15);
    for (const Grid<int>::Payload& i : range) { found.insert(i.get()); }

    EXPECT_NE(found.find(87), found.end());
}

TEST(Grid, IterateCell) {
    Grid<int> grid(100, 100, 10, 10);

    grid.add(5, 5, 52);
    grid.add(95, 64, 23);
    grid.emplace(99, 23, 87);

    std::unordered_set<int> found;
    auto range = grid.getCell(3, 8);
    for (const Grid<int>::Payload& i : range) { found.insert(i.get()); }

    EXPECT_NE(found.find(52), found.end());
}

TEST(Grid, IterateCellAndNeighbors) {
    Grid<int> grid(100, 100, 10, 10);

    // not neighbor
    grid.add(5, 5, 52);
    grid.add(95, 64, 23);
    grid.emplace(99, 23, 87);

    // neighbors
    grid.add(51, 51, 1);
    grid.add(61, 51, 2);
    grid.add(41, 51, 3);
    grid.add(51, 61, 4);
    grid.add(51, 41, 5);

    std::unordered_set<int> found;
    auto range = grid.getCellAndNeighbors(55, 55);
    for (const Grid<int>::Payload& i : range) { found.insert(i.get()); }

    EXPECT_NE(found.find(1), found.end());
    EXPECT_NE(found.find(2), found.end());
    EXPECT_NE(found.find(3), found.end());
    EXPECT_NE(found.find(4), found.end());
    EXPECT_NE(found.find(5), found.end());
}

TEST(Grid, Clear) {
    Grid<int> grid(100, 100, 10, 10);

    grid.add(5, 5, 52);
    grid.add(95, 64, 23);
    grid.emplace(99, 23, 87);

    std::vector<std::weak_ptr<Grid<int>::Payload>> payloads;
    for (Grid<int>::Payload& p : grid.getAll()) { payloads.push_back(p.weak_from_this()); }
    grid.clear();

    for (auto& p : payloads) { EXPECT_TRUE(p.expired()); }
    EXPECT_EQ(grid.getAll().begin(), grid.getAll().end());
}

TEST(Grid, Move) {
    Grid<int> grid(100, 100, 10, 10);

    auto p = grid.add(5, 5, 10);
    p->move(55, 55);

    EXPECT_EQ(grid.getCell(5, 5).begin(), grid.getCell(5, 5).end());
    ASSERT_NE(grid.getCell(55, 55).begin(), grid.getCell(55, 55).end());
    EXPECT_EQ(grid.getCell(55, 55).begin()->get(), 10);
}

TEST(Grid, RemoveOnly) {
    Grid<int> grid(100, 100, 10, 10);

    auto p = grid.add(5, 5, 10);
    p->remove();

    EXPECT_EQ(grid.getCell(5, 5).begin(), grid.getCell(5, 5).end());
    EXPECT_EQ(grid.getAll().begin(), grid.getAll().end());
}

TEST(Grid, RemoveHead) {
    Grid<int> grid(100, 100, 10, 10);

    auto p1 = grid.add(5, 5, 5);
    auto p2 = grid.add(5, 5, 10);
    auto p3 = grid.add(5, 5, 15);
    p3->remove();

    std::unordered_set<int> found;
    auto range = grid.getCell(3, 8);
    for (const Grid<int>::Payload& i : range) { found.insert(i.get()); }

    EXPECT_NE(found.find(5), found.end());
    EXPECT_NE(found.find(10), found.end());
}

TEST(Grid, RemoveMiddle) {
    Grid<int> grid(100, 100, 10, 10);

    auto p1 = grid.add(5, 5, 5);
    auto p2 = grid.add(5, 5, 10);
    auto p3 = grid.add(5, 5, 15);
    p2->remove();

    std::unordered_set<int> found;
    auto range = grid.getCell(3, 8);
    for (const Grid<int>::Payload& i : range) { found.insert(i.get()); }

    EXPECT_NE(found.find(5), found.end());
    EXPECT_NE(found.find(15), found.end());
}

TEST(Grid, RemoveEnd) {
    Grid<int> grid(100, 100, 10, 10);

    auto p1 = grid.add(5, 5, 5);
    auto p2 = grid.add(5, 5, 10);
    auto p3 = grid.add(5, 5, 15);
    p1->remove();

    std::unordered_set<int> found;
    auto range = grid.getCell(3, 8);
    for (const Grid<int>::Payload& i : range) { found.insert(i.get()); }

    EXPECT_NE(found.find(15), found.end());
    EXPECT_NE(found.find(10), found.end());
}

} // namespace unittest
} // namespace container
} // namespace bl
