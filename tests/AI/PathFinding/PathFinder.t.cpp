#include <BLIB/AI/PathFinding.hpp>
#include <BLIB/Util/Hashes.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace ai
{
namespace unittest
{
namespace
{
const int map[6][6] = {{0, 1, 0, 0, 0, 0},
                       {1, 1, 0, 1, 1, 0},
                       {0, 1, 0, 1, 1, 1},
                       {0, 1, 0, 0, 0, 0},
                       {0, 1, 1, 1, 1, 0},
                       {0, 0, 0, 0, 0, 0}};

struct TestMap : public PathMap<sf::Vector2i> {
    virtual ~TestMap() = default;

    virtual int movementCost(const sf::Vector2i&, const sf::Vector2i& to,
                             ecs::Entity) const override {
        if (to.x < 0 || to.x >= 6 || to.y < 0 || to.y >= 6) return -1;
        if (map[to.y][to.x] == 0) return 1;
        return -1;
    }

    virtual void getAdjacentNodes(const sf::Vector2i& node,
                                  std::vector<sf::Vector2i>& result) const override {
        result.push_back({node.x, node.y - 1});
        result.push_back({node.x, node.y + 1});
        result.push_back({node.x - 1, node.y});
        result.push_back({node.x + 1, node.y});
    }
};
} // namespace

TEST(PathFinding, FindPath) {
    PathFinder<sf::Vector2i, util::Vector2Hash<int>> finder(helpers::Vector2ManhattanDist<int>);
    std::vector<sf::Vector2i> path;
    EXPECT_TRUE(finder.findPath({0, 2}, {0, 5}, 5, TestMap(), path));
}

TEST(PathFinding, NoPath) {
    PathFinder<sf::Vector2i, util::Vector2Hash<int>> finder(helpers::Vector2ManhattanDist<int>);
    std::vector<sf::Vector2i> path;
    EXPECT_FALSE(finder.findPath({0, 0}, {1, 5}, 5, TestMap(), path));
}

TEST(PathFinding, SpecificPath) {
    PathFinder<sf::Vector2i, util::Vector2Hash<int>> finder(helpers::Vector2ManhattanDist<int>);
    std::vector<sf::Vector2i> path;
    EXPECT_TRUE(finder.findPath({0, 2}, {5, 1}, 5, TestMap(), path));
    ASSERT_EQ(path.size(), 20);
    EXPECT_EQ(path[0], sf::Vector2i(0, 3));
    EXPECT_EQ(path[1], sf::Vector2i(0, 4));
    EXPECT_EQ(path[2], sf::Vector2i(0, 5));
    EXPECT_EQ(path[3], sf::Vector2i(1, 5));
    EXPECT_EQ(path[4], sf::Vector2i(2, 5));
    EXPECT_EQ(path[5], sf::Vector2i(3, 5));
    EXPECT_EQ(path[6], sf::Vector2i(4, 5));
    EXPECT_EQ(path[7], sf::Vector2i(5, 5));
    EXPECT_EQ(path[8], sf::Vector2i(5, 4));
    EXPECT_EQ(path[9], sf::Vector2i(5, 3));
    EXPECT_EQ(path[10], sf::Vector2i(4, 3));
    EXPECT_EQ(path[11], sf::Vector2i(3, 3));
    EXPECT_EQ(path[12], sf::Vector2i(2, 3));
    EXPECT_EQ(path[13], sf::Vector2i(2, 2));
    EXPECT_EQ(path[14], sf::Vector2i(2, 1));
    EXPECT_EQ(path[15], sf::Vector2i(2, 0));
    EXPECT_EQ(path[16], sf::Vector2i(3, 0));
    EXPECT_EQ(path[17], sf::Vector2i(4, 0));
    EXPECT_EQ(path[18], sf::Vector2i(5, 0));
    EXPECT_EQ(path[19], sf::Vector2i(5, 1));
}

} // namespace unittest
} // namespace ai
} // namespace bl
