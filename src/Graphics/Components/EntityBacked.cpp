#include <BLIB/Graphics/Components/EntityBacked.hpp>

#include <BLIB/Engine.hpp>

namespace bl
{
namespace gfx
{
namespace bcom
{
EntityBacked::EntityBacked()
: enginePtr(nullptr)
, ecsId(ecs::InvalidEntity) {}

EntityBacked::EntityBacked(EntityBacked&& move)
: enginePtr(move.enginePtr)
, ecsId(move.ecsId) {
    move.enginePtr = nullptr;
    move.ecsId     = ecs::InvalidEntity;
}

EntityBacked::~EntityBacked() { destroy(); }

EntityBacked& EntityBacked::operator=(EntityBacked&& move) {
    destroy();

    enginePtr      = move.enginePtr;
    ecsId          = move.ecsId;
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
    ecsId     = engine.ecs().createEntity(flags);
}

void EntityBacked::setParent(const EntityBacked& parent) {
    enginePtr->ecs().setEntityParent(ecsId, parent.ecsId);
}

void EntityBacked::setParent(ecs::Entity parent) {
    enginePtr->ecs().setEntityParent(ecsId, parent);
}

void EntityBacked::removeParent() { enginePtr->ecs().removeEntityParent(ecsId); }

} // namespace bcom
} // namespace gfx
} // namespace bl
