#include <BLIB/Render/Cameras/Camera.hpp>

namespace bl
{
namespace render
{
namespace camera
{
Camera::Camera()
: viewport(0.f, 0.f, 800.f, 600.f)
, rotation(0.f) {}

Camera::Camera(const sf::FloatRect& vp, float r)
: viewport(vp)
, rotation(r) {}

const sf::FloatRect& Camera::getViewport() const { return viewport; }

sf::Vector2f Camera::getCenter() const {
    return {viewport.left + viewport.width * 0.5f, viewport.top + viewport.height * 0.5f};
}

sf::Vector2f Camera::getSize() const { return {viewport.width, viewport.height}; }

void Camera::setCenter(const sf::Vector2f& pos) {
    viewport.left = pos.x - viewport.width * 0.5f;
    viewport.top  = pos.y - viewport.height * 0.5f;
}

void Camera::setSize(const sf::Vector2f& size) {
    viewport.width  = size.x;
    viewport.height = size.y;
}

void Camera::setZoomLevel(float zoom, const sf::Vector2f& size) {
    viewport.width  = size.x * zoom;
    viewport.height = size.y * zoom;
}

void Camera::zoom(float f) {
    viewport.width *= f;
    viewport.height *= f;
}

void Camera::setViewport(const sf::FloatRect& vp) { viewport = vp; }

float Camera::getRotation() const { return rotation; }

void Camera::setRotation(float r) { rotation = r; }

void Camera::rotate(float r) { rotation += r; }

} // namespace camera
} // namespace render
} // namespace bl
