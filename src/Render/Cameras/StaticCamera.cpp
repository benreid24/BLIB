#include <BLIB/Render/Cameras/StaticCamera.hpp>

namespace bl
{
namespace render
{
namespace camera
{
StaticCamera::Ptr StaticCamera::create(const sf::Vector2f& size, float r) {
    return Ptr{new StaticCamera({sf::Vector2f{0.f, 0.f}, size}, r)};
}

StaticCamera::Ptr StaticCamera::create(const sf::FloatRect& vp, float r) {
    return Ptr{new StaticCamera(vp, r)};
}

StaticCamera::StaticCamera(const sf::FloatRect& vp, float r)
: Camera(vp, r) {}

void StaticCamera::update(float) {}

bool StaticCamera::valid() const { return true; }

} // namespace camera
} // namespace render
} // namespace bl
