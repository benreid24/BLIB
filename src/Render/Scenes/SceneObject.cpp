#include <BLIB/Render/Scenes/SceneObject.hpp>

#include <BLIB/Render/Components/DrawableBase.hpp>

namespace bl
{
namespace rc
{
namespace scene
{
SceneObject::SceneObject()
: entity(ecs::InvalidEntity)
, component(nullptr) {}

void SceneObject::updateRefToThis() {
    if (component) { component->sceneRef.object = this; }
}

} // namespace scene
} // namespace rc
} // namespace bl
