#include <BLIB/Render/Components/DrawableBase.hpp>

#include <BLIB/Components/Rendered.hpp>
#include <BLIB/ECS/Registry.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>

namespace bl
{
namespace rc
{
namespace rcom
{
DrawableBase::DrawableBase()
: pipeline(PipelineNotSet)
, containsTransparency(false)
, renderComponent(nullptr)
, hidden(false) {}

DrawableBase::~DrawableBase() {
    if (renderComponent) { renderComponent->component = nullptr; }
}

void DrawableBase::syncDrawParamsToScene() {
    if (sceneRef.object) {
        sceneRef.object->drawParams = drawParams;
        sceneRef.object->hidden     = hidden;
    }
}

void DrawableBase::setHidden(bool hide) {
    if (sceneRef.object) { sceneRef.object->hidden = hide; }
    hidden = hide;
}

void DrawableBase::setPipeline(std::uint32_t pid) {
    if (pipeline != pid) {
        pipeline = pid;
        rebucket();
    }
}

void DrawableBase::rebucket() {
    if (sceneRef.scene) { sceneRef.scene->rebucketObject(*this); }
}

void DrawableBase::setContainsTransparency(bool t) {
    if (containsTransparency != t) {
        containsTransparency = t;
        rebucket();
    }
}

void DrawableBase::addToScene(ecs::Registry& ecs, ecs::Entity entity, Scene* scene,
                              UpdateSpeed updateSpeed) {
    if (pipeline == PipelineNotSet) {
        pipeline = dynamic_cast<rc::Overlay*>(scene) != nullptr ? getDefaultOverlayPipelineId() :
                                                                  getDefaultScenePipelineId();
    }
    addToSceneWithPipeline(ecs, entity, scene, updateSpeed, pipeline);
}

void DrawableBase::addToSceneWithPipeline(ecs::Registry& ecs, ecs::Entity entity, Scene* scene,
                                          UpdateSpeed updateSpeed, std::uint32_t pid) {
    if (sceneRef.scene) {
        if (scene == sceneRef.scene) { return; }
        removeFromScene(ecs, entity);
        sceneRef.scene = nullptr;
    }

    pipeline = pid;
    renderComponent =
        ecs.emplaceComponent<com::Rendered>(entity, entity, *this, scene, updateSpeed);
}

void DrawableBase::removeFromScene(ecs::Registry& ecs, ecs::Entity entity) {
    ecs.removeComponent<com::Rendered>(entity);
    renderComponent = nullptr;
    sceneRef.object = nullptr;
    sceneRef.scene  = nullptr;
}

} // namespace rcom
} // namespace rc
} // namespace bl
