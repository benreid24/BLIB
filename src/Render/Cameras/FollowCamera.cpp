#include <BLIB/Render/Cameras/FollowCamera.hpp>

namespace bl
{
namespace render
{
namespace camera
{
FollowCamera::Ptr FollowCamera::create(sf::Vector2f* fp, const sf::Vector2f& size) {
    return Ptr{new FollowCamera(fp, size)};
}

FollowCamera::FollowCamera(sf::Vector2f* fp, const sf::Vector2f& size)
: follow(fp) {
    setSize(size);
    if (follow) { setCenter(*fp); }
}

void FollowCamera::update(float) {
    if (follow) { setCenter(*follow); }
}

bool FollowCamera::valid() const { return follow != nullptr; }

} // namespace camera
} // namespace render
} // namespace bl
