#include <BLIB/Entities.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace entity
{
namespace unittest
{
namespace
{
struct Component1 {
    static constexpr Component::IdType ComponentId = 1;
    int value;
};

struct Component2 {
    static constexpr Component::IdType ComponentId = 2;
    std::string value;

    Component2(const std::string& s)
    : value(s) {}

    Component2& operator=(const Component2& c) {
        value = c.value;
        return *this;
    }
};
} // namespace

TEST(Registry, Empty) {
    Registry registry;
    EXPECT_EQ(registry.begin(), registry.end());
}

TEST(Registry, EntityAdd) {
    Registry registry;
    registry.createEntity();
    auto beg = registry.begin();
    EXPECT_NE(beg, registry.end());
    ++beg;
    EXPECT_EQ(beg, registry.end());
}

TEST(Registry, NoComponents) {
    Registry registry;
    Entity e = registry.createEntity();
    EXPECT_FALSE(registry.hasComponent<Component1>(e));
    EXPECT_EQ(registry.getComponent<Component1>(e), nullptr);
    auto view = registry.getEntitiesWithComponents<Component1>();
    EXPECT_EQ(view->results().size(), 0);
    auto cs = registry.getEntityComponents<Component1>(e);
    EXPECT_EQ(cs.get<Component1>(), nullptr);
}

TEST(Registry, SingleComponent) {
    Registry registry;
    Entity e = registry.createEntity();
    ASSERT_TRUE(registry.addComponent<Component1>(e, {5}));
    EXPECT_FALSE(registry.addComponent<Component1>(e, {5}));

    EXPECT_TRUE(registry.hasComponent<Component1>(e));
    ASSERT_NE(registry.getComponent<Component1>(e), nullptr);
    EXPECT_EQ(registry.getComponent<Component1>(e)->value, 5);
    auto view = registry.getEntitiesWithComponents<Component1>();
    EXPECT_EQ(view->results().size(), 1);
    auto cs = registry.getEntityComponents<Component1>(e);
    EXPECT_NE(cs.get<Component1>(), nullptr);
}

TEST(Registry, EntityDelete) {
    Registry registry;
    Entity e = registry.createEntity();
    ASSERT_TRUE(registry.addComponent<Component1>(e, {5}));
    EXPECT_FALSE(registry.addComponent<Component1>(e, {5}));

    registry.destroyEntity(e);
    EXPECT_EQ(registry.begin(), registry.end());

    EXPECT_FALSE(registry.hasComponent<Component1>(e));
    EXPECT_EQ(registry.getComponent<Component1>(e), nullptr);
    auto view = registry.getEntitiesWithComponents<Component1>();
    EXPECT_EQ(view->results().size(), 0);
    auto cs = registry.getEntityComponents<Component1>(e);
    EXPECT_EQ(cs.get<Component1>(), nullptr);
}

TEST(Registry, ComponentDelete) {
    Registry registry;
    Entity e = registry.createEntity();
    ASSERT_TRUE(registry.addComponent<Component1>(e, {5}));
    EXPECT_FALSE(registry.addComponent<Component1>(e, {5}));
    EXPECT_TRUE(registry.removeComponent<Component1>(e));

    EXPECT_FALSE(registry.hasComponent<Component1>(e));
    EXPECT_EQ(registry.getComponent<Component1>(e), nullptr);
    auto view = registry.getEntitiesWithComponents<Component1>();
    EXPECT_EQ(view->results().size(), 0);
    auto cs = registry.getEntityComponents<Component1>(e);
    EXPECT_EQ(cs.get<Component1>(), nullptr);
}

TEST(Registry, MultipleEntities) {
    Registry registry;
    Entity e1 = registry.createEntity();
    Entity e2 = registry.createEntity();

    ASSERT_TRUE(registry.addComponent<Component1>(e1, {5}));
    ASSERT_TRUE(registry.addComponent<Component1>(e2, {10}));

    EXPECT_TRUE(registry.hasComponent<Component1>(e1));
    ASSERT_NE(registry.getComponent<Component1>(e1), nullptr);
    EXPECT_EQ(registry.getComponent<Component1>(e1)->value, 5);
    auto cs1 = registry.getEntityComponents<Component1>(e1);
    EXPECT_NE(cs1.get<Component1>(), nullptr);
    EXPECT_EQ(cs1.get<Component1>()->value, 5);

    EXPECT_TRUE(registry.hasComponent<Component1>(e2));
    ASSERT_NE(registry.getComponent<Component1>(e2), nullptr);
    EXPECT_EQ(registry.getComponent<Component1>(e2)->value, 10);
    auto cs2 = registry.getEntityComponents<Component1>(e2);
    EXPECT_NE(cs2.get<Component1>(), nullptr);
    EXPECT_EQ(cs2.get<Component1>()->value, 10);

    auto view = registry.getEntitiesWithComponents<Component1>();
    EXPECT_EQ(view->results().size(), 2);
}

TEST(Registry, MultipleComponents) {
    Registry registry;
    Entity e = registry.createEntity();
    ASSERT_TRUE(registry.addComponent<Component1>(e, {5}));
    ASSERT_TRUE(registry.addComponent<Component2>(e, {"hello"}));

    EXPECT_TRUE(registry.hasComponent<Component1>(e));
    ASSERT_NE(registry.getComponent<Component1>(e), nullptr);
    EXPECT_EQ(registry.getComponent<Component1>(e)->value, 5);
    auto view1 = registry.getEntitiesWithComponents<Component1>();
    EXPECT_EQ(view1->results().size(), 1);
    auto cs1 = registry.getEntityComponents<Component1>(e);
    EXPECT_NE(cs1.get<Component1>(), nullptr);

    EXPECT_TRUE(registry.hasComponent<Component2>(e));
    ASSERT_NE(registry.getComponent<Component2>(e), nullptr);
    EXPECT_EQ(registry.getComponent<Component2>(e)->value, "hello");
    auto view2 = registry.getEntitiesWithComponents<Component2>();
    EXPECT_EQ(view2->results().size(), 1);
    auto cs2 = registry.getEntityComponents<Component2>(e);
    EXPECT_NE(cs2.get<Component2>(), nullptr);

    auto view = registry.getEntitiesWithComponents<Component1, Component2>();
    EXPECT_EQ(view->results().size(), 1);

    auto cs = registry.getEntityComponents<Component1, Component2>(e);
    EXPECT_NE(cs.get<Component1>(), nullptr);
    EXPECT_NE(cs.get<Component2>(), nullptr);
}

TEST(Registry, ComponentSearch) {
    Registry registry;
    Entity e1 = registry.createEntity();
    Entity e2 = registry.createEntity();
    ASSERT_TRUE(registry.addComponent<Component1>(e1, {5}));
    ASSERT_TRUE(registry.addComponent<Component1>(e2, {5}));
    ASSERT_TRUE(registry.addComponent<Component2>(e1, {"hello"}));

    auto view1 = registry.getEntitiesWithComponents<Component1>();
    EXPECT_EQ(view1->results().size(), 2);

    auto view2 = registry.getEntitiesWithComponents<Component1, Component2>();
    EXPECT_EQ(view2->results().size(), 1);
}

TEST(Registry, ViewUpdate) {
    Registry registry;
    Entity e1 = registry.createEntity();
    Entity e2 = registry.createEntity();

    auto view = registry.getEntitiesWithComponents<Component1>();
    EXPECT_EQ(view->results().size(), 0);

    registry.addComponent<Component1>(e1, {5});
    EXPECT_EQ(view->results().size(), 1);

    registry.addComponent<Component1>(e2, {5});
    EXPECT_EQ(view->results().size(), 2);

    registry.removeComponent<Component1>(e1);
    EXPECT_EQ(view->results().size(), 1);

    registry.destroyEntity(e2);
    EXPECT_EQ(view->results().size(), 0);
}

TEST(Registry, ViewIterate) {
    Registry registry;
    Entity e1 = registry.createEntity();
    Entity e2 = registry.createEntity();
    ASSERT_TRUE(registry.addComponent<Component1>(e1, {5}));
    ASSERT_TRUE(registry.addComponent<Component1>(e2, {5}));

    auto view = registry.getEntitiesWithComponents<Component1>();
    int s     = 0;
    for (auto& set : *view) { s += set.get<Component1>()->value; }
    EXPECT_EQ(s, 10);
}

TEST(Registry, PredefinedEntity) {
    Registry registry;
    Entity e1 = registry.createEntity();
    EXPECT_FALSE(registry.createEntity(e1));
    EXPECT_TRUE(registry.createEntity(e1 + 1));
}

} // namespace unittest
} // namespace entity
} // namespace bl
