#include <BLIB/Render/Cameras/3D/CameraAffector3D.hpp>

namespace bl
{
namespace rc
{
namespace c3d
{
void CameraAffector3D::update(float, bool&, bool&) {}

void CameraAffector3D::applyOnView(glm::vec3&, t3d::Orientation3D&) {}

void CameraAffector3D::applyOnProjection(float&, float&, float&) {}

} // namespace c3d
} // namespace rc
} // namespace bl
