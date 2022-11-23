#include <BLIB/AI/PathFinder.hpp>
#include <BLIB/Math.hpp>
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

const auto nodeCallback = [](const sf::Vector2i& node,
                             std::vector<std::pair<sf::Vector2i, int>>& adj) {
    adj.reserve(4);
    adj.emplace_back(sf::Vector2i{node.x, node.y - 1}, 1);
    adj.emplace_back(sf::Vector2i{node.x, node.y + 1}, 1);
    adj.emplace_back(sf::Vector2i{node.x - 1, node.y}, 1);
    adj.emplace_back(sf::Vector2i{node.x + 1, node.y}, 1);
    for (int i = 3; i >= 0; --i) {
        const auto& p = adj[i].first;
        if (p.x < 0 || p.y < 0 || p.x >= 6 || p.y >= 6) {
            adj.erase(adj.begin() + i);
            continue;
        }
        if (map[p.y][p.x] != 0) { adj.erase(adj.begin() + i); }
    }
};

int (*const distCallback)(const sf::Vector2i&, const sf::Vector2i&) = &math::manhattanDistance<int>;

using TestFinder = PathFinder<sf::Vector2i, util::Vector2Hash<int>>;

} // namespace

TEST(PathFinding, FindPath) {
    std::vector<sf::Vector2i> path;
    EXPECT_TRUE(TestFinder::findPath({0, 2}, {0, 5}, nodeCallback, distCallback, path));
}

TEST(PathFinding, NoPath) {
    std::vector<sf::Vector2i> path;
    EXPECT_FALSE(TestFinder::findPath({0, 0}, {1, 5}, nodeCallback, distCallback, path));
}

TEST(PathFinding, SpecificPath) {
    std::vector<sf::Vector2i> path;
    EXPECT_TRUE(TestFinder::findPath({0, 2}, {5, 1}, nodeCallback, distCallback, path));
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
