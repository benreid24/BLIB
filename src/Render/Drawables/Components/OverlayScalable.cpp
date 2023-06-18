#include <BLIB/Render/Drawables/Components/OverlayScalable.hpp>

#include <BLIB/Events.hpp>
#include <BLIB/Render/Drawables/Components/OverlayScalable.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <BLIB/Render/Overlays/ViewportChanged.hpp>
#include <BLIB/Render/Systems/OverlayScaler.hpp>

namespace bl
{
namespace render
{
namespace draw
{
namespace base
{
OverlayScalable::OverlayScalable()
: registry(nullptr)
, ecsId(ecs::InvalidEntity)
, overlay(nullptr) {}

void OverlayScalable::setViewport(const ovy::Viewport& vp) {
    registry->emplaceComponent<ovy::Viewport>(ecsId, vp);
    if (overlay) { event::Dispatcher::dispatch(ovy::ViewportChanged(overlay, sceneId)); }
}

void OverlayScalable::setViewportToSelf() {
    auto cs = registry->getComponentSet<com::OverlayScaler, t2d::Transform2D>(ecsId);
#ifdef BLIB_DEBUG
    if (!cs.isValid()) {
        BL_LOG_ERROR << "Tried to set viewport to self for un-initialized entity";
        return;
    }
#endif
    scalerSystem->processEntity(cs);

    com::OverlayScaler& c  = handle.get();
    const glm::vec2& pos   = Transform2D::getTransform().getPosition();
    const glm::vec2& scale = Transform2D::getTransform().getScale();
    setViewport(
        ovy::Viewport::relative({pos.x / c.cachedOverlaySize.x,
                                 pos.y / c.cachedOverlaySize.y,
                                 (c.cachedObjectSize.x * scale.x) / c.cachedOverlaySize.x,
                                 (c.cachedObjectSize.y * scale.y) / c.cachedOverlaySize.y}));

    // TODO - account for origin
    Transform2D::getTransform().setPosition({0.f, 0.f});
    Transform2D::getTransform().setScale({c.cachedOverlaySize.x / c.cachedObjectSize.x,
                                          c.cachedOverlaySize.y / c.cachedObjectSize.y});
}

void OverlayScalable::clearViewport() { registry->removeComponent<ovy::Viewport>(ecsId); }

glm::vec2 OverlayScalable::getOverlaySize() const {
    const glm::vec2& ls    = getLocalSize();
    const glm::vec2& scale = Transform2D::getTransform().getScale();
    return {ls.x * scale.x, ls.y * scale.y};
}

glm::vec2 OverlayScalable::getScreenSize() const {
    const glm::vec2 os          = getOverlaySize();
    const com::OverlayScaler& c = handle.get();
    return {os.x / c.cachedOverlaySize.x * c.cachedTargetSize.x,
            os.y / c.cachedOverlaySize.y * c.cachedTargetSize.y};
}

void OverlayScalable::notifySceneAdd(const com::SceneObjectRef& si) {
    scalerSystem->queueScalerSceneAdd(this, si);
}

void OverlayScalable::notifySceneRemove() {
    overlay = nullptr;
    sceneId = 0;
}

} // namespace base
} // namespace draw
} // namespace render
} // namespace bl
