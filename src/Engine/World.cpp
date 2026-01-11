#include <BLIB/Engine/World.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace engine
{
World::~World() { destroyAllEntities(); }

ecs::Entity World::createEntity(ecs::Flags flags) { return owner.ecs().createEntity(index, flags); }

ecs::Entity World::createEntity(const ecs::Transaction<ecs::tx::EntityWrite>& transaction,
                                ecs::Flags flags) {
    return owner.ecs().createEntity(index, flags, transaction);
}

void World::destroyAllEntities() { owner.ecs().destroyEntitiesInWorld(index); }

} // namespace engine
} // namespace bl
