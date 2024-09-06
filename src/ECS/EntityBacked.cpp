#include <BLIB/ECS/EntityBacked.hpp>

#include <BLIB/Engine.hpp>

namespace bl
{
namespace ecs
{
EntityBacked::EntityBacked()
: enginePtr(nullptr)
, ecsId(ecs::InvalidEntity)
, isDestroyed(true) {}

EntityBacked::EntityBacked(EntityBacked&& move)
: enginePtr(move.enginePtr)
, ecsId(move.ecsId)
, isDestroyed(move.isDestroyed) {
    move.enginePtr = nullptr;
    move.ecsId     = ecs::InvalidEntity;
}

EntityBacked::~EntityBacked() {
    if (isDestroyed) { destroy(); }
}

EntityBacked& EntityBacked::operator=(EntityBacked&& move) {
    destroy();

    enginePtr      = move.enginePtr;
    ecsId          = move.ecsId;
    isDestroyed    = move.isDestroyed;
    move.enginePtr = nullptr;
    move.ecsId     = ecs::InvalidEntity;

    return *this;
}

void EntityBacked::destroy() {
    if (enginePtr && ecsId != ecs::InvalidEntity) {
        enginePtr->ecs().destroyEntity(ecsId);
        ecsId     = ecs::InvalidEntity;
        enginePtr = nullptr;
    }
}

void EntityBacked::createEntityOnly(engine::Engine& engine, ecs::Flags flags) {
    enginePtr = &engine;
    // TODO - WORLD_UPDATE - update to take world instead
    ecsId = engine.ecs().createEntity(0, flags);
}

void EntityBacked::createFromExistingEntity(engine::Engine& engine, ecs::Entity ent) {
    enginePtr = &engine;
    ecsId     = ent;
}

void EntityBacked::deleteEntityOnDestroy(bool d) { isDestroyed = d; }

void EntityBacked::setParent(const EntityBacked& parent) {
    enginePtr->ecs().setEntityParent(ecsId, parent.ecsId);
}

void EntityBacked::setParent(ecs::Entity parent) {
    enginePtr->ecs().setEntityParent(ecsId, parent);
}

void EntityBacked::removeParent() { enginePtr->ecs().removeEntityParent(ecsId); }

bool EntityBacked::exists() const {
    return enginePtr && ecsId != InvalidEntity && enginePtr->ecs().entityExists(ecsId);
}

} // namespace ecs
} // namespace bl
