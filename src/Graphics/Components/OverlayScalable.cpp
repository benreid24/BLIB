#include <BLIB/Graphics/Components/OverlayScalable.hpp>

#include <BLIB/Events.hpp>
#include <BLIB/Graphics/Components/OverlayScalable.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <BLIB/Systems/OverlayScalerSystem.hpp>

namespace bl
{
namespace gfx
{
namespace bcom
{
OverlayScalable::OverlayScalable()
: registry(nullptr)
, ecsId(ecs::InvalidEntity) {}

void OverlayScalable::setViewport(const rc::ovy::Viewport& vp) {
    registry->emplaceComponent<rc::ovy::Viewport>(ecsId, vp);
}

void OverlayScalable::setViewportToSelf(bool setToSelf) {
    handle.get().setViewportToSelf(setToSelf);
}

void OverlayScalable::clearViewport() { registry->removeComponent<rc::ovy::Viewport>(ecsId); }

glm::vec2 OverlayScalable::getOverlaySize() const {
    const glm::vec2& ls    = getLocalSize();
    const glm::vec2& scale = Transform2D::getTransform().getScale();
    return {ls.x * scale.x, ls.y * scale.y};
}

glm::vec2 OverlayScalable::getTargetSize() const {
    const com::OverlayScaler& c = handle.get();
    return {c.cachedTargetRegion.width, c.cachedTargetRegion.height};
}

} // namespace bcom
} // namespace gfx
} // namespace bl
