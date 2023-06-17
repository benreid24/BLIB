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
