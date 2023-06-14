#include <BLIB/Render/Drawables/Components/Transform2D.hpp>

namespace bl
{
namespace render
{
namespace draw
{
namespace base
{
void Transform2D::scaleWidthToOverlay(float w, float ow) {
    const float ar = localSize.x / localSize.y;
    const float xs = w / localSize.x * ow;
    getTransform().setScale({xs, xs * ar});
}

void Transform2D::scaleHeightToOverlay(float h, float oh) {
    const float ar = localSize.y / localSize.x;
    const float xy = h / localSize.y * oh;
    getTransform().setScale({xy * ar, xy});
}

void Transform2D::scaleToOverlay(const glm::vec2& r, const glm::vec2& os) {
    getTransform().setScale({r.x / localSize.x * os.x, r.y / localSize.y * os.y});
}

void Transform2D::setLocalSize(const glm::vec2& ls) { localSize = ls; }

} // namespace base
} // namespace draw
} // namespace render
} // namespace bl
