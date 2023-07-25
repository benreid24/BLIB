#include <BLIB/Containers/QuadTree.hpp>
#include <SFML/System/Vector2.hpp>
#include <gtest/gtest.h>
#include <set>

namespace bl
{
namespace ctr
{
namespace unittest
{
namespace
{
template<class Container>
std::string printSet(const Container& v) {
    if (v.empty()) return "[]";

    std::stringstream ss;
    ss << "[";
    for (int i : v) { ss << i << ", "; }
    ss << "]";

    return ss.str();
}

void compareResults(QuadTree<sf::Vector2i, int>::ResultSet actual, std::set<int> expected) {
    std::vector<int> expanded;
    for (int i : actual) { expanded.push_back(i); }

    const std::vector<int> ogActual = expanded;
    const std::set<int> ogExpected  = expected;

    EXPECT_EQ(expanded.size(), expected.size());
    for (unsigned int i = 0; i < expanded.size(); ++i) {
        auto it = expected.find(expanded[i]);
        if (it != expected.end()) {
            expected.erase(it);
            expanded.erase(expanded.begin() + i);
            --i;
        }
    }

    EXPECT_TRUE(expected.empty()) << "Expected " << printSet(ogExpected) << " got "
                                  << printSet(ogActual);
}
} // namespace

TEST(QuadTree, IterateAll) {
    QuadTree<sf::Vector2i, int> tree;
    tree.setIndexedArea({{-50, 0}, {75, 20}});
    tree.add({5, 5}, 12);
    tree.add({23, 13}, 57);
    tree.add({-50, 14}, 90);

    auto set = tree.all();
    std::set<int> found({12, 57, 90});
    for (int val : set) { found.erase(val); }
    EXPECT_TRUE(found.empty());
}

TEST(QuadTree, Remove) {
    QuadTree<sf::Vector2i, int> tree;
    tree.add({10, 10}, 10);
    tree.add({12, 12}, 56);
    tree.remove({10, 10});

    auto set = tree.all();
    std::set<int> found({10, 56});
    for (int val : set) { found.erase(val); }
    ASSERT_EQ(found.size(), 1);
    EXPECT_EQ(*found.begin(), 10);
}

TEST(QuadTree, BasicPartition) {
    QuadTree<sf::Vector2i, int> tree;
    tree.setIndexedArea({{0, 0}, {10, 10}});
    tree.setMaxLoad(1);
    tree.add({3, 3}, 5);
    tree.add({7, 7}, 34);

    compareResults(tree.getQuad({2, 2}), {5});
    compareResults(tree.getQuad({8, 8}), {34});
    compareResults(tree.getQuad({7, 2}), {});
    compareResults(tree.getQuad({2, 7}), {});
}

TEST(QuadTree, UpdatePosition) {
    QuadTree<sf::Vector2i, int> tree;
    tree.setIndexedArea({{0, 0}, {10, 10}});
    tree.setMaxLoad(1);
    tree.add({3, 3}, 5);
    tree.add({7, 7}, 34);
    tree.updatePosition({7, 7}, {2, 8});

    compareResults(tree.getQuad({2, 2}), {5});
    compareResults(tree.getQuad({8, 8}), {});
    compareResults(tree.getQuad({7, 2}), {});
    compareResults(tree.getQuad({2, 7}), {34});
}

TEST(QuadTree, SearchArea) {
    QuadTree<sf::Vector2i, int> tree;
    tree.setIndexedArea({{0, 0}, {10, 10}});
    tree.setMaxLoad(1);
    tree.add({3, 3}, 5);
    tree.add({7, 7}, 34);

    compareResults(tree.getInArea({{3, 3}, {3, 3}}), {5, 34});
}

TEST(QuadTree, CombinePartitions) {
    QuadTree<sf::Vector2i, int> tree;
    tree.setIndexedArea({{0, 0}, {20, 20}});
    tree.setMaxLoad(2);

    tree.add({3, 3}, 3);
    tree.add({7, 7}, 7);
    tree.add({8, 9}, 9);

    compareResults(tree.getQuad({7, 7}), {7, 9});
    compareResults(tree.getQuad({3, 3}), {3});

    tree.remove({8, 9});
    compareResults(tree.getQuad({5, 5}), {3, 7});
}

TEST(QuadTree, Repartition) {
    QuadTree<sf::Vector2i, int> tree;
    tree.setIndexedArea({{0, 0}, {20, 20}});
    tree.setMaxLoad(2);

    tree.add({3, 3}, 3);
    tree.add({7, 7}, 7);
    tree.add({8, 9}, 9);

    compareResults(tree.getQuad({7, 7}), {7, 9});
    compareResults(tree.getQuad({3, 3}), {3});

    tree.setMaxLoad(3);
    compareResults(tree.getQuad({7, 7}), {7, 9, 3});
}

TEST(QuadTree, Clear) {
    QuadTree<sf::Vector2i, int> tree;
    tree.setIndexedArea({{0, 0}, {20, 20}});
    tree.setMaxLoad(2);

    tree.add({3, 3}, 3);
    tree.add({7, 7}, 7);
    tree.add({8, 9}, 9);
    tree.clear();

    compareResults(tree.getQuad({7, 7}), {});
}

} // namespace unittest
} // namespace ctr
} // namespace bl
