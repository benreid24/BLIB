#include <BLIB/Render/Cameras/FollowCamera.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace render
{
namespace camera
{
FollowCamera::Ptr FollowCamera::create(const sf::Vector2f* fp, const sf::Vector2f& size) {
    if (fp == nullptr) { BL_LOG_WARN << "Creating FollowCamera for nullptr position"; }
    return Ptr{new FollowCamera(fp, size)};
}

FollowCamera::FollowCamera(const sf::Vector2f* fp, const sf::Vector2f& size)
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
