#include <BLIB/Render/Cameras/Camera.hpp>

namespace bl
{
namespace render
{
Camera::Camera()
: view(1.f)
, viewDirty(true)
, proj(1.f)
, projDirty(true)
, vpWidth(0.f)
, vpHeight(0.f) {}

const glm::mat4& Camera::getViewMatrix() {
    if (viewDirty) {
        refreshViewMatrix(view);
        viewDirty = false;
    }
    return view;
}

const glm::mat4& Camera::getProjectionMatrix(const VkViewport& viewport) {
    if (projDirty || vpWidth != viewport.width || vpHeight != viewport.height) {
        refreshProjMatrix(proj, viewport);
        projDirty = false;
        vpWidth   = viewport.width;
        vpHeight  = viewport.height;
    }
    return proj;
}

void Camera::markViewDirty() { viewDirty = true; }

void Camera::markProjDirty() { projDirty = true; }

} // namespace render
} // namespace bl
