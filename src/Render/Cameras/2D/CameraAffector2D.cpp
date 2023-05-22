#include <BLIB/Render/Cameras/2D/CameraAffector2D.hpp>

namespace bl
{
namespace render
{
namespace c2d
{
void CameraAffector2D::update(float, bool&, bool&) {}

void CameraAffector2D::applyOnView(float&) {}

void CameraAffector2D::applyOnProj(glm::vec2&, glm::vec2&) {}

} // namespace c2d
} // namespace render
} // namespace bl
