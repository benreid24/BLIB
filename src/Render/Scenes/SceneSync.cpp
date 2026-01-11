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
: registry(r) {
    ecsRouter.route<ecs::event::ComponentAdded<com::Rendered>>(
        [this](const ecs::event::ComponentAdded<com::Rendered>& event) {
            handleComponentAdd(event);
        });
    ecsRouter.route<ecs::event::ComponentRemoved<com::Rendered>>(
        [this](const ecs::event::ComponentRemoved<com::Rendered>& event) {
            handleComponentRemove(event);
        });
    renderRouter.route<rc::event::SceneObjectRemoved>(
        [this](const rc::event::SceneObjectRemoved& event) { handleSceneRemove(event); });
    renderRouter.route<rc::event::SceneDestroyed>(
        [this](const rc::event::SceneDestroyed& event) { handleSceneDestroy(event); });
}

void SceneSync::subscribe(sig::Channel& renderChanel) {
    ecsRouter.subscribe(registry.getSignalChannel());
    renderRouter.subscribe(renderChanel);
}

void SceneSync::unsubscribe() {
    ecsRouter.unsubscribe();
    renderRouter.unsubscribe();
}

void SceneSync::handleComponentAdd(const ecs::event::ComponentAdded<com::Rendered>& event) {
    event.component.getScene()->createAndAddObject(
        event.entity, *event.component.getComponent(), event.component.getUpdateSpeed());
}

void SceneSync::handleComponentRemove(const ecs::event::ComponentRemoved<com::Rendered>& event) {
    if (event.component.getScene()) {
        auto* scene = event.component.getScene();
        const_cast<com::Rendered&>(event.component).invalidate(); // prevent delete cycle
        if (event.component.getComponent() &&
            event.component.getComponent()->getSceneRef().object) {
            auto& sceneRef =
                const_cast<rcom::SceneObjectRef&>(event.component.getComponent()->getSceneRef());
            scene->removeObject(sceneRef.object);
            sceneRef.scene                                  = nullptr;
            sceneRef.object                                 = nullptr;
            event.component.getComponent()->renderComponent = nullptr;
        }
    }
}

void SceneSync::handleSceneRemove(const rc::event::SceneObjectRemoved& event) {
    com::Rendered* r = registry.getComponent<com::Rendered>(event.entity);
    if (r && r->getScene()) {
        r->invalidate();
        if (r->getComponent()) {
            r->getComponent()->sceneRef.scene  = nullptr;
            r->getComponent()->sceneRef.object = nullptr;
            r->getComponent()->renderComponent = nullptr;
        }
        registry.removeComponent<com::Rendered>(event.entity);
    }
}

void SceneSync::handleSceneDestroy(const rc::event::SceneDestroyed& event) {
    std::vector<ecs::Entity> toRm;
    registry.getAllComponents<com::Rendered>().forEach(
        [this, &event, &toRm](ecs::Entity ent, com::Rendered& r) {
            if (r.getScene() == event.scene) {
                toRm.reserve(128);
                r.invalidate();
                if (r.getComponent()) {
                    r.getComponent()->sceneRef.scene  = nullptr;
                    r.getComponent()->sceneRef.object = nullptr;
                    r.getComponent()->renderComponent = nullptr;
                }
                toRm.emplace_back(ent);
            }
        });
    for (auto ent : toRm) { registry.removeComponent<com::Rendered>(ent); }
}

} // namespace scene
} // namespace rc
} // namespace bl
