#include <BLIB/ECS/ParentGraph.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace ecs
{
namespace unittest
{
TEST(ECSParentGraph, BasicParenting) {
    const Entity child  = IdUtil::composeEntity(0, 1);
    const Entity parent = IdUtil::composeEntity(1, 1);

    ParentGraph graph(0);

    EXPECT_EQ(graph.getParent(child), InvalidEntity);
    EXPECT_EQ(graph.getChildren(parent).begin(), graph.getChildren(parent).end());

    graph.setParent(child, parent);
    EXPECT_EQ(graph.getParent(child), parent);
    EXPECT_NE(graph.getChildren(parent).begin(), graph.getChildren(parent).end());
    EXPECT_EQ(*graph.getChildren(parent).begin(), child);

    graph.unParent(child);
    EXPECT_EQ(graph.getParent(child), InvalidEntity);
    EXPECT_EQ(graph.getChildren(parent).begin(), graph.getChildren(parent).end());
}

} // namespace unittest
} // namespace ecs
} // namespace bl
