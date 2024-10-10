#include <BLIB/ECS.hpp>
#include <BLIB/Util/Random.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace ecs
{
namespace unittest
{
namespace
{
constexpr std::size_t MaxEntities = 100;

struct DestroyTestComponent {
    DestroyTestComponent(bool& b)
    : destroyed(b) {
        destroyed = false;
    }

    ~DestroyTestComponent() { destroyed = true; }

    bool& destroyed;
};

struct ParentTestComponent
: public trait::ParentAware<ParentTestComponent>
, public trait::ChildAware<ParentTestComponent> {
    int payload;

    ParentTestComponent(int p)
    : payload(p) {}
};

} // namespace

TEST(ECS, EntityCreateAndDestroy) {
    Registry testRegistry;

    Entity e = testRegistry.createEntity(0);
    ASSERT_TRUE(testRegistry.entityExists(e));

    testRegistry.destroyEntity(e);
    testRegistry.flushDeletions();
    EXPECT_FALSE(testRegistry.entityExists(e));
}

TEST(ECS, EntitySingleComponent) {
    Registry testRegistry;

    Entity e = testRegistry.createEntity(0);
    int* c   = testRegistry.addComponent<int>(e, 5);
    ASSERT_NE(c, nullptr);
    EXPECT_EQ(*c, 5);

    *c = 15;
    c  = testRegistry.getComponent<int>(e);
    ASSERT_NE(c, nullptr);
    EXPECT_EQ(*c, 15);

    testRegistry.removeComponent<int>(e);
    testRegistry.flushDeletions();
    EXPECT_EQ(testRegistry.getComponent<int>(e), nullptr);
}

TEST(ECS, MultipleEntitiesSingleComponent) {
    Registry testRegistry;

    Entity e1 = testRegistry.createEntity(0);
    Entity e2 = testRegistry.createEntity(0);

    int* c1 = testRegistry.addComponent<int>(e1, 10);
    int* c2 = testRegistry.addComponent<int>(e2, 20);
    ASSERT_NE(c1, c2);
    EXPECT_EQ(*c1, 10);
    EXPECT_EQ(*c2, 20);

    testRegistry.destroyEntity(e1);
    testRegistry.flushDeletions();
    int* c2a = testRegistry.getComponent<int>(e2);
    EXPECT_EQ(c2, c2a);
    EXPECT_EQ(testRegistry.getComponent<int>(e1), nullptr);
}

TEST(ECS, EntityMultipleComponents) {
    Registry testRegistry;

    Entity e = testRegistry.createEntity(0);
    int* ca  = testRegistry.addComponent<int>(e, 5);
    char* cb = testRegistry.addComponent<char>(e, 'a');
    ASSERT_NE(ca, nullptr);
    EXPECT_EQ(*ca, 5);
    ASSERT_NE(cb, nullptr);
    EXPECT_EQ(*cb, 'a');

    *ca = 15;
    *cb = 'b';
    ca  = testRegistry.getComponent<int>(e);
    cb  = testRegistry.getComponent<char>(e);
    ASSERT_NE(ca, nullptr);
    EXPECT_EQ(*ca, 15);
    ASSERT_NE(cb, nullptr);
    EXPECT_EQ(*cb, 'b');

    testRegistry.removeComponent<int>(e);
    testRegistry.flushDeletions();
    EXPECT_EQ(testRegistry.getComponent<int>(e), nullptr);

    testRegistry.removeComponent<char>(e);
    testRegistry.flushDeletions();
    EXPECT_EQ(testRegistry.getComponent<char>(e), nullptr);
}

TEST(ECS, MultipleEntitiesMultipleComponents) {
    Registry testRegistry;

    Entity e1 = testRegistry.createEntity(0);
    Entity e2 = testRegistry.createEntity(0);

    int* c1a = testRegistry.addComponent<int>(e1, 10);
    int* c2a = testRegistry.addComponent<int>(e2, 20);
    ASSERT_NE(c1a, c2a);
    EXPECT_EQ(*c1a, 10);
    EXPECT_EQ(*c2a, 20);

    char* c1b = testRegistry.addComponent<char>(e1, 'a');
    char* c2b = testRegistry.addComponent<char>(e2, 'b');
    ASSERT_NE(c1b, c2b);
    EXPECT_EQ(*c1b, 'a');
    EXPECT_EQ(*c2b, 'b');

    testRegistry.destroyEntity(e1);
    testRegistry.flushDeletions();
    int* c2a2 = testRegistry.getComponent<int>(e2);
    EXPECT_EQ(c2a, c2a2);
    char* c2b2 = testRegistry.getComponent<char>(e2);
    EXPECT_EQ(c2b, c2b2);
    EXPECT_EQ(testRegistry.getComponent<int>(e1), nullptr);
    EXPECT_EQ(testRegistry.getComponent<char>(e1), nullptr);
}

TEST(ECS, ComponentDestroyed) {
    Registry testRegistry;

    bool destroyed = false;
    Entity e       = testRegistry.createEntity(0);
    testRegistry.emplaceComponent<DestroyTestComponent>(e, std::ref(destroyed));
    testRegistry.removeComponent<DestroyTestComponent>(e);
    testRegistry.flushDeletions();
    EXPECT_TRUE(destroyed);

    destroyed = false;
    testRegistry.addComponent<DestroyTestComponent>(e, std::move(DestroyTestComponent{destroyed}));
    testRegistry.destroyEntity(e);
    testRegistry.flushDeletions();
    EXPECT_TRUE(destroyed);

    destroyed = false;
    e         = testRegistry.createEntity(0);
    testRegistry.addComponent<DestroyTestComponent>(e, {destroyed});
    testRegistry.destroyAllEntities();
    EXPECT_TRUE(destroyed);
}

TEST(ECS, ComponentIterate) {
    Registry testRegistry;

    std::unordered_map<Entity, int> values;
    for (unsigned int i = 0; i < MaxEntities; ++i) {
        const Entity e = testRegistry.createEntity(0);
        const int v    = util::Random::get<int>(0, 100000);
        testRegistry.addComponent<int>(e, v);
        values.emplace(e, v);
    }

    const auto visitor = [&values](Entity e, int& c) {
        const auto it = values.find(e);
        ASSERT_NE(it, values.end());
        EXPECT_EQ(c, it->second);
        values.erase(it);
    };

    testRegistry.getAllComponents<int>().forEach(visitor);
    EXPECT_TRUE(values.empty());
}

TEST(ECS, ViewIterate) {
    Registry testRegistry;

    // create entities and components
    std::unordered_map<Entity, std::pair<int, char>> ogValues;
    std::unordered_map<Entity, std::pair<int, char>> afterRmValues;
    std::unordered_map<Entity, std::pair<int, char>> afterAddValues;
    std::vector<Entity> toRemove;
    std::vector<Entity> toAdd;
    for (unsigned int i = 0; i < MaxEntities; ++i) {
        const Entity e = testRegistry.createEntity(0);
        const int v    = util::Random::get<int>(0, 100000);
        testRegistry.addComponent<int>(e, v);
        if (util::Random::get<int>(0, 100) < 50) {
            const char cv = util::Random::get<char>('a', 'z');
            testRegistry.addComponent<char>(e, cv);
            ogValues.emplace(e, std::make_pair(v, cv));
            if (util::Random::get<int>(0, 100) < 50) { toRemove.push_back(e); }
            else { afterRmValues.emplace(e, std::make_pair(v, cv)); }
        }
        else { toAdd.push_back(e); }
    }
    afterAddValues = ogValues;

    // create view
    auto* view      = testRegistry.getOrCreateView<Require<int, char>>();
    auto* viewCheck = testRegistry.getOrCreateView<Require<int, char>>();
    ASSERT_EQ(view, viewCheck);

    const auto visitor = [](std::unordered_map<Entity, std::pair<int, char>>& values,
                            ComponentSet<Require<int, char>>& cs) {
        const auto it = values.find(cs.entity());
        ASSERT_NE(it, values.end());
        EXPECT_EQ(it->second.first, *cs.get<int>());
        EXPECT_EQ(it->second.second, *cs.get<char>());
        values.erase(it);
    };

    // test original view
    view->forEach(std::bind(visitor, std::ref(ogValues), std::placeholders::_1));
    EXPECT_TRUE(ogValues.empty());

    // add components and retest view
    for (const Entity ent : toAdd) {
        const char cv = util::Random::get<char>('a', 'z');
        testRegistry.addComponent<char>(ent, cv);
        int* ic = testRegistry.getComponent<int>(ent);
        ASSERT_NE(ic, nullptr);
        afterAddValues.emplace(ent, std::make_pair(*ic, cv));
        afterRmValues.emplace(ent, std::make_pair(*ic, cv));
    }
    view->forEach(std::bind(visitor, std::ref(afterAddValues), std::placeholders::_1));
    EXPECT_TRUE(afterAddValues.empty());

    // remove components and entities and retest view
    for (const Entity ent : toRemove) {
        if (util::Random::get<int>(0, 100) < 50) { testRegistry.removeComponent<char>(ent); }
        else { testRegistry.destroyEntity(ent); }
    }
    testRegistry.flushDeletions();
    view->forEach(std::bind(visitor, std::ref(afterRmValues), std::placeholders::_1));
    EXPECT_TRUE(afterRmValues.empty());
}

TEST(ECS, ViewOptionalExclude) {
    Registry testRegistry;

    auto* view = testRegistry.getOrCreateView<Require<int>, Optional<char>, Exclude<short>>();
    using CSet = typename std::remove_pointer_t<decltype(view)>::TRow;

    // req and opt
    const Entity e1 = testRegistry.createEntity(0);
    testRegistry.addComponent<int>(e1, 5);
    testRegistry.addComponent<char>(e1, 'a');

    // req but excluded
    const Entity e2 = testRegistry.createEntity(0);
    testRegistry.addComponent<int>(e2, 10);
    testRegistry.addComponent<short>(e2, 4);

    // req no opt
    const Entity e3 = testRegistry.createEntity(0);
    testRegistry.addComponent<int>(e3, 15);

    // verify e1 and e3 are in view
    view->forEach([e1, e3](CSet& cset) {
        ASSERT_TRUE(cset.entity() == e1 || cset.entity() == e3);
        if (cset.entity() == e1) {
            EXPECT_EQ(*cset.get<int>(), 5);
            EXPECT_EQ(*cset.get<char>(), 'a');
        }
        else {
            EXPECT_EQ(*cset.get<int>(), 15);
            EXPECT_EQ(cset.get<char>(), nullptr);
        }
    });

    // add exclude to e3 to remove it
    testRegistry.addComponent<short>(e3, 45);
    view->forEach([e1](CSet& cset) {
        ASSERT_EQ(cset.entity(), e1);
        if (cset.entity() == e1) {
            EXPECT_EQ(*cset.get<int>(), 5);
            EXPECT_EQ(*cset.get<char>(), 'a');
        }
    });
}

TEST(ECS, FillComponentSet) {
    Registry testRegistry;

    Entity e = testRegistry.createEntity(0);
    testRegistry.addComponent<int>(e, 5);
    testRegistry.addComponent<char>(e, 'g');

    auto cs = testRegistry.getComponentSet<Require<int, char>>(e);
    ASSERT_TRUE(cs.isValid());
    EXPECT_EQ(*cs.get<int>(), 5);
    EXPECT_EQ(*cs.get<char>(), 'g');
}

TEST(ECS, ComponentSetOptional) {
    Registry testRegistry;

    Entity e = testRegistry.createEntity(0);
    testRegistry.addComponent<int>(e, 5);

    auto cs = testRegistry.getComponentSet<Require<int>, Optional<char>>(e);
    ASSERT_TRUE(cs.isValid());
    EXPECT_EQ(*cs.get<int>(), 5);
    EXPECT_EQ(cs.get<char>(), nullptr);
}

TEST(ECS, ClearRegistry) {
    Registry testRegistry;

    std::vector<Entity> ents;
    ents.reserve(MaxEntities);
    for (unsigned int i = 0; i < MaxEntities; ++i) {
        ents.push_back(testRegistry.createEntity(0));
        testRegistry.addComponent<int>(ents.back(), util::Random::get<int>(0, 10000));
    }

    testRegistry.destroyAllEntities();
    for (const Entity ent : ents) {
        EXPECT_FALSE(testRegistry.entityExists(ent));
        EXPECT_EQ(testRegistry.getComponent<int>(ent), nullptr);
    }
}

TEST(ECS, ParentBeforeCreate) {
    Registry testRegistry;

    Entity child  = testRegistry.createEntity(0);
    Entity parent = testRegistry.createEntity(0);
    testRegistry.setEntityParent(child, parent);

    auto* parentCom = testRegistry.addComponent<ParentTestComponent>(parent, 100);
    auto* childCom  = testRegistry.addComponent<ParentTestComponent>(child, 10);

    EXPECT_TRUE(childCom->hasParent());
    EXPECT_FALSE(parentCom->hasParent());
    EXPECT_EQ(parentCom->getChildren().size(), 1);
    EXPECT_EQ(childCom->payload, 10);
    EXPECT_EQ(parentCom->payload, 100);
    EXPECT_EQ(childCom->getParent().payload, 100);

    testRegistry.destroyEntity(parent);
    testRegistry.flushDeletions();
    EXPECT_FALSE(testRegistry.entityExists(parent));
    EXPECT_FALSE(testRegistry.entityExists(child));
}

TEST(ECS, DestroyParentComponent) {
    Registry testRegistry;

    Entity child  = testRegistry.createEntity(0);
    Entity parent = testRegistry.createEntity(0);
    testRegistry.setEntityParent(child, parent);

    auto* parentCom = testRegistry.addComponent<ParentTestComponent>(parent, 100);
    auto* childCom  = testRegistry.addComponent<ParentTestComponent>(child, 10);

    EXPECT_TRUE(childCom->hasParent());
    EXPECT_FALSE(parentCom->hasParent());
    EXPECT_EQ(parentCom->getChildren().size(), 1);
    EXPECT_EQ(childCom->payload, 10);
    EXPECT_EQ(parentCom->payload, 100);
    EXPECT_EQ(childCom->getParent().payload, 100);

    testRegistry.removeComponent<ParentTestComponent>(parent);
    testRegistry.flushDeletions();
    EXPECT_FALSE(childCom->hasParent());

    parentCom = testRegistry.addComponent<ParentTestComponent>(parent, 100);
    EXPECT_TRUE(childCom->hasParent());
    EXPECT_FALSE(parentCom->hasParent());
    EXPECT_EQ(parentCom->getChildren().size(), 1);
    EXPECT_EQ(childCom->payload, 10);
    EXPECT_EQ(parentCom->payload, 100);
    EXPECT_EQ(childCom->getParent().payload, 100);

    testRegistry.removeComponent<ParentTestComponent>(child);
    testRegistry.flushDeletions();
    EXPECT_FALSE(parentCom->hasParent());
    EXPECT_EQ(parentCom->getChildren().size(), 0);
    EXPECT_EQ(parentCom->payload, 100);
}

TEST(ECS, ParentAfterCreate) {
    Registry testRegistry;

    Entity child  = testRegistry.createEntity(0);
    Entity parent = testRegistry.createEntity(0);

    auto* childCom  = testRegistry.addComponent<ParentTestComponent>(child, 10);
    auto* parentCom = testRegistry.addComponent<ParentTestComponent>(parent, 100);
    testRegistry.setEntityParent(child, parent);

    EXPECT_TRUE(childCom->hasParent());
    EXPECT_FALSE(parentCom->hasParent());
    EXPECT_EQ(parentCom->getChildren().size(), 1);
    EXPECT_EQ(childCom->payload, 10);
    EXPECT_EQ(parentCom->payload, 100);
    EXPECT_EQ(childCom->getParent().payload, 100);

    testRegistry.destroyEntity(parent);
    testRegistry.flushDeletions();
    EXPECT_FALSE(testRegistry.entityExists(parent));
    EXPECT_FALSE(testRegistry.entityExists(child));
}

TEST(ECS, ParentRemove) {
    Registry testRegistry;

    Entity child  = testRegistry.createEntity(0);
    Entity parent = testRegistry.createEntity(0);

    auto* childCom  = testRegistry.addComponent<ParentTestComponent>(child, 10);
    auto* parentCom = testRegistry.addComponent<ParentTestComponent>(parent, 100);
    testRegistry.setEntityParent(child, parent);

    EXPECT_TRUE(childCom->hasParent());
    EXPECT_FALSE(parentCom->hasParent());
    EXPECT_EQ(childCom->payload, 10);
    EXPECT_EQ(parentCom->payload, 100);
    EXPECT_EQ(childCom->getParent().payload, 100);

    testRegistry.removeEntityParent(child);
    EXPECT_FALSE(childCom->hasParent());

    testRegistry.destroyEntity(parent);
    testRegistry.flushDeletions();
    EXPECT_FALSE(testRegistry.entityExists(parent));
    EXPECT_TRUE(testRegistry.entityExists(child));
}

TEST(ECS, ChildComponentReAdd) {
    Registry testRegistry;

    Entity child  = testRegistry.createEntity(0);
    Entity parent = testRegistry.createEntity(0);

    auto* childCom  = testRegistry.addComponent<ParentTestComponent>(child, 10);
    auto* parentCom = testRegistry.addComponent<ParentTestComponent>(parent, 100);
    testRegistry.setEntityParent(child, parent);

    ASSERT_EQ(parentCom->getChildren().front(), childCom);

    childCom = testRegistry.addComponent<ParentTestComponent>(child, 10);
    ASSERT_EQ(parentCom->getChildren().front(), childCom);
}

TEST(ECS, Dependencies) {
    Registry testRegistry;

    Entity child  = testRegistry.createEntity(0);
    Entity parent = testRegistry.createEntity(0);

    // test marked removal
    testRegistry.addDependency(parent, child);
    EXPECT_TRUE(testRegistry.isDependedOn(parent));
    EXPECT_FALSE(testRegistry.destroyEntity(parent));
    EXPECT_TRUE(testRegistry.destroyEntity(child));
    testRegistry.flushDeletions();
    EXPECT_FALSE(testRegistry.entityExists(parent));

    // test add and remove dep
    child  = testRegistry.createEntity(0);
    parent = testRegistry.createEntity(0);
    testRegistry.addDependency(parent, child);
    testRegistry.removeDependency(parent, child);
    EXPECT_TRUE(testRegistry.destroyEntity(parent));
    EXPECT_TRUE(testRegistry.destroyEntity(child));
    testRegistry.flushDeletions();

    // test remove in order
    child  = testRegistry.createEntity(0);
    parent = testRegistry.createEntity(0);
    testRegistry.addDependency(parent, child);
    EXPECT_TRUE(testRegistry.destroyEntity(child));
    EXPECT_TRUE(testRegistry.destroyEntity(parent));
    testRegistry.flushDeletions();

    // test delete on remove
    child  = testRegistry.createEntity(0);
    parent = testRegistry.createEntity(0);
    testRegistry.addDependency(parent, child);
    testRegistry.removeDependencyAndDestroyIfPossible(parent, child);
    testRegistry.flushDeletions();
    EXPECT_FALSE(testRegistry.entityExists(parent));
}

namespace
{
struct ParentVersionedComponent : public trait::ParentAwareVersioned<ParentVersionedComponent> {
    void refresh() {
        if (hasParent()) { getParent().refresh(); }
        markRefreshed();
    }
};
} // namespace

TEST(ECS, ParentAwareVersionedTrait) {
    Registry testRegistry;

    Entity child  = testRegistry.createEntity(0);
    Entity parent = testRegistry.createEntity(0);

    ParentVersionedComponent& childCom =
        *testRegistry.emplaceComponent<ParentVersionedComponent>(child);
    ParentVersionedComponent& parentCom =
        *testRegistry.emplaceComponent<ParentVersionedComponent>(parent);

    // verify components are dirty to start
    EXPECT_TRUE(childCom.refreshRequired());
    EXPECT_TRUE(parentCom.refreshRequired());

    // verify that refreshing marks as clean
    childCom.refresh();
    parentCom.refresh();
    EXPECT_FALSE(childCom.refreshRequired());
    EXPECT_FALSE(parentCom.refreshRequired());

    // verify that incrementing version sets dirty
    childCom.incrementVersion();
    EXPECT_TRUE(childCom.refreshRequired());
    childCom.refresh();
    EXPECT_FALSE(childCom.refreshRequired());

    // set parent and verify that child is dirty
    testRegistry.setEntityParent(child, parent);
    EXPECT_TRUE(childCom.refreshRequired());
    childCom.refresh();
    EXPECT_FALSE(childCom.refreshRequired());

    // verify that marking parent as dirty sets child dirty
    parentCom.incrementVersion();
    EXPECT_TRUE(childCom.refreshRequired());
    childCom.refresh();
    EXPECT_FALSE(childCom.refreshRequired());
    EXPECT_FALSE(parentCom.refreshRequired());

    // un-parent and verify that child gets set dirty
    testRegistry.removeEntityParent(child);
    EXPECT_TRUE(childCom.refreshRequired());
    childCom.refresh();
    EXPECT_FALSE(childCom.refreshRequired());
}

TEST(ECS, DestroyByFlags) {
    Registry testRegistry;

    Entity toBeDestroyed[]       = {testRegistry.createEntity(0, Flags::WorldObject),
                                    testRegistry.createEntity(0, Flags::WorldObject),
                                    testRegistry.createEntity(0, Flags::WorldObject),
                                    testRegistry.createEntity(0, Flags::WorldObject | Flags::Dummy),
                                    testRegistry.createEntity(0, Flags::WorldObject),
                                    testRegistry.createEntity(0, Flags::WorldObject | Flags::Dummy)};
    const std::size_t nDestroyed = std::size(toBeDestroyed);

    Entity toLive[]         = {testRegistry.createEntity(0, Flags::None),
                               testRegistry.createEntity(0, Flags::Dummy),
                               testRegistry.createEntity(0, Flags::Dummy),
                               testRegistry.createEntity(0, Flags::Dummy),
                               testRegistry.createEntity(0, Flags::None)};
    const std::size_t nLive = std::size(toLive);

    EXPECT_EQ(testRegistry.destroyAllWorldEntities(), nDestroyed);
    testRegistry.flushDeletions();
    for (std::size_t i = 0; i < nDestroyed; ++i) {
        EXPECT_FALSE(testRegistry.entityExists(toBeDestroyed[i]));
    }
    for (std::size_t i = 0; i < nLive; ++i) { EXPECT_TRUE(testRegistry.entityExists(toLive[i])); }
}

TEST(ECS, DestroyInWorld) {
    Registry testRegistry;

    Entity toBeDestroyed[]       = {testRegistry.createEntity(0, Flags::WorldObject),
                                    testRegistry.createEntity(0, Flags::WorldObject),
                                    testRegistry.createEntity(0, Flags::WorldObject),
                                    testRegistry.createEntity(0, Flags::WorldObject | Flags::Dummy),
                                    testRegistry.createEntity(0, Flags::WorldObject),
                                    testRegistry.createEntity(0, Flags::WorldObject | Flags::Dummy)};
    const std::size_t nDestroyed = std::size(toBeDestroyed);

    Entity toLive[]         = {testRegistry.createEntity(1, Flags::None),
                               testRegistry.createEntity(1, Flags::Dummy),
                               testRegistry.createEntity(1, Flags::Dummy),
                               testRegistry.createEntity(1, Flags::Dummy),
                               testRegistry.createEntity(1, Flags::None)};
    const std::size_t nLive = std::size(toLive);

    EXPECT_EQ(testRegistry.destroyEntitiesInWorld(0), nDestroyed);
    testRegistry.flushDeletions();
    for (std::size_t i = 0; i < nDestroyed; ++i) {
        EXPECT_FALSE(testRegistry.entityExists(toBeDestroyed[i]));
    }
    for (std::size_t i = 0; i < nLive; ++i) { EXPECT_TRUE(testRegistry.entityExists(toLive[i])); }
}

namespace
{
struct RemoveEventCounter : public bl::event::Listener<event::ComponentRemoved<int>> {
    unsigned int count;

    RemoveEventCounter()
    : count(0) {}

    virtual void observe(const event::ComponentRemoved<int>&) { ++count; }
};
} // namespace

TEST(ECS, ComponentRemovedEvents) {
    Registry testRegistry;
    RemoveEventCounter counter;
    bl::event::Dispatcher::subscribe(&counter);

    Entity entity = testRegistry.createEntity(0);
    testRegistry.addComponent<int>(entity, 5);
    testRegistry.removeComponent<int>(entity);
    testRegistry.destroyEntity(entity);
    testRegistry.flushDeletions();
    EXPECT_EQ(counter.count, 1);

    counter.count = 0;
    entity        = testRegistry.createEntity(0);
    testRegistry.addComponent<int>(entity, 5);
    testRegistry.destroyEntity(entity);
    testRegistry.removeComponent<int>(entity);
    testRegistry.flushDeletions();
    EXPECT_EQ(counter.count, 1);
}

} // namespace unittest
} // namespace ecs
} // namespace bl
