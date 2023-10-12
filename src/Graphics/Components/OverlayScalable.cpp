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
, ecsId(ecs::InvalidEntity)
, handle(nullptr) {}

void OverlayScalable::setScissorToSelf(bool setToSelf) { handle->setScissorToSelf(setToSelf); }

glm::vec2 OverlayScalable::getOverlaySize() const {
    const glm::vec2& ls    = getLocalSize();
    const glm::vec2& scale = Transform2D::getTransform().getScale();
    return {ls.x * scale.x, ls.y * scale.y};
}

} // namespace bcom
} // namespace gfx
} // namespace bl
