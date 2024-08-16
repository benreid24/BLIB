#include <BLIB/Render/Scenes/SceneSync.hpp>

#include <BLIB/ECS/Registry.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>

namespace bl
{
namespace rc
{
namespace scene
{
SceneSync::SceneSync(ecs::Registry& r)
: registry(r) {}

void SceneSync::observe(const ecs::event::ComponentAdded<com::Rendered>& event) {
    event.component.getScene()->createAndAddObject(
        event.entity, *event.component.getComponent(), event.component.getUpdateSpeed());
}

void SceneSync::observe(const ecs::event::ComponentRemoved<com::Rendered>& event) {
    if (event.component.getScene()) {
        auto* scene = event.component.getScene();
        const_cast<com::Rendered&>(event.component).invalidate(); // prevent delete cycle
        scene->removeObject(event.component.getComponent()->getSceneRef().object);
    }
}

void SceneSync::observe(const rc::event::SceneObjectRemoved& event) {
    com::Rendered* r = registry.getComponent<com::Rendered>(event.entity);
    if (r && r->getScene()) {
        r->invalidate();
        registry.removeComponent<com::Rendered>(event.entity);
    }
}

void SceneSync::observe(const rc::event::SceneDestroyed& event) {
    registry.getAllComponents<com::Rendered>().forEachWithWrites(
        [this, &event](ecs::Entity ent, com::Rendered& r) {
            if (r.getScene() == event.scene) {
                r.invalidate();
                r.getComponent()->sceneRef.scene = nullptr;
                registry.removeComponent<com::Rendered>(ent);
            }
        });
}

} // namespace scene
} // namespace rc
} // namespace bl
