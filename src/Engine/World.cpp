#include <BLIB/Engine/World.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace engine
{
World::~World() { destroyAllEntities(); }

ecs::Entity World::createEntity(ecs::Flags flags) { return owner.ecs().createEntity(index, flags); }

void World::destroyAllEntities() { owner.ecs().destroyEntitiesInWorld(index); }

} // namespace engine
} // namespace bl
