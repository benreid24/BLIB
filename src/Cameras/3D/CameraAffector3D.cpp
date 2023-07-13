#include <BLIB/Cameras/3D/CameraAffector3D.hpp>

namespace bl
{
namespace cam
{
void CameraAffector3D::update(float, bool&, bool&) {}

void CameraAffector3D::applyOnView(glm::vec3&, com::Orientation3D&) {}

void CameraAffector3D::applyOnProjection(float&, float&, float&) {}

} // namespace cam
} // namespace bl
