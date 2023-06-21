#include <BLIB/Render/Drawables/Components/OverlayScalable.hpp>

#include <BLIB/Events.hpp>
#include <BLIB/Render/Drawables/Components/OverlayScalable.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
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
, ecsId(ecs::InvalidEntity) {}

void OverlayScalable::setViewport(const ovy::Viewport& vp) {
    registry->emplaceComponent<ovy::Viewport>(ecsId, vp);
}

void OverlayScalable::setViewportToSelf(bool setToSelf) {
    handle.get().setViewportToSelf(setToSelf);
}

void OverlayScalable::clearViewport() { registry->removeComponent<ovy::Viewport>(ecsId); }

glm::vec2 OverlayScalable::getOverlaySize() const {
    const glm::vec2& ls    = getLocalSize();
    const glm::vec2& scale = Transform2D::getTransform().getScale();
    return {ls.x * scale.x, ls.y * scale.y};
}

glm::vec2 OverlayScalable::getTargetSize() const {
    const com::OverlayScaler& c = handle.get();
    return {c.cachedTargetRegion.width, c.cachedTargetRegion.height};
}

} // namespace base
} // namespace draw
} // namespace render
} // namespace bl
