#include <BLIB/Render/Overlays/OverlayCamera.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace bl
{
namespace rc
{
namespace ovy
{
OverlayCamera::OverlayCamera()
: Camera() {
    markViewDirty();
    markProjDirty();
}

void OverlayCamera::update(float) {}

void OverlayCamera::refreshViewMatrix(glm::mat4& view) { view = glm::mat4(1.f); }

void OverlayCamera::refreshProjMatrix(glm::mat4& proj, const VkViewport&) {
    proj = glm::ortho(0.f, 1.f, 0.f, 1.f);
}

} // namespace ovy
} // namespace rc
} // namespace bl
