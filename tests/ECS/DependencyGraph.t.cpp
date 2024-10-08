#include <BLIB/ECS/DependencyGraph.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace ecs
{
namespace unittest
{
TEST(ECSDependencyGraph, BasicDependencies) {
    const Entity resource = Entity(0, 1);
    const Entity user1    = Entity(1, 1);
    const Entity user2    = Entity(1, 1);

    DependencyGraph graph(10);
    EXPECT_FALSE(graph.hasDependencies(resource));
    graph.addDependency(resource, user1);
    EXPECT_TRUE(graph.hasDependencies(resource));
    graph.addDependency(resource, user2);
    EXPECT_TRUE(graph.hasDependencies(resource));

    graph.removeDependency(resource, user1);
    EXPECT_TRUE(graph.hasDependencies(resource));

    graph.removeDependency(resource, user2);
    EXPECT_FALSE(graph.hasDependencies(resource));
}

} // namespace unittest
} // namespace ecs
} // namespace bl
