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
, ecsId(ecs::InvalidEntity)
, handle(nullptr) {}

void OverlayScalable::setScissorToSelf(bool setToSelf) {
    handle->setScissorMode(setToSelf ? com::OverlayScaler::ScissorSelf :
                                       com::OverlayScaler::ScissorInherit);
}

glm::vec2 OverlayScalable::getLocalSize() const {
    const_cast<OverlayScalable*>(this)->ensureLocalSizeUpdated();
    const sf::FloatRect& bounds = handle->getEntityBounds();
    return {bounds.width, bounds.height};
}

glm::vec2 OverlayScalable::getGlobalSize() const {
    const glm::vec2 ls     = getLocalSize();
    const glm::vec2& scale = Transform2D::getTransform().getScale();
    return {ls.x * scale.x, ls.y * scale.y};
}

} // namespace bcom
} // namespace gfx
} // namespace bl
