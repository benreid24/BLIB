#include <BLIB/Render/Cameras/2D/Camera2D.hpp>

#include <BLIB/Render/Config.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace bl
{
namespace render
{
namespace c2d
{
Camera2D::Camera2D(const glm::vec2& center, const glm::vec2& size, float rotation)
: center(center)
, size(size)
, rotation(rotation) {}

Camera2D::Camera2D(const sf::FloatRect& area, float rotation)
: rotation(rotation) {
    setVisibleArea(area);
}

void Camera2D::setCenter(const glm::vec2& c) {
    center = c;
    markViewDirty();
}

void Camera2D::move(const glm::vec2& offset) {
    center += offset;
    markViewDirty();
}

void Camera2D::setSize(const glm::vec2& ns) {
    size = ns;
    markViewDirty();
}

void Camera2D::zoom(float factor) {
    size.x *= factor;
    size.y *= factor;
    markViewDirty();
}

void Camera2D::zoom(const glm::vec2& factors) {
    size.x *= factors.x;
    size.y *= factors.y;
    markViewDirty();
}

void Camera2D::changeSize(const glm::vec2& delta) {
    size += delta;
    markViewDirty();
}

void Camera2D::setCorner(const glm::vec2& c) {
    center = c + size * 0.5f;
    markViewDirty();
}

glm::vec2 Camera2D::getCorner() const { return center - size * 0.5f; }

void Camera2D::setVisibleArea(const sf::FloatRect& area) {
    size.x = area.width;
    size.y = area.height;
    setCorner({area.left, area.top});
}

sf::FloatRect Camera2D::getVisibleArea() const {
    return {center.x - size.x * 0.5f, center.y - size.y * 0.5f, size.x, size.y};
}

void Camera2D::setRotation(float angle) {
    rotation = angle;
    markViewDirty();
}

void Camera2D::rotate(float d) {
    rotation += d;
    markViewDirty();
}

void Camera2D::removeAffector(const CameraAffector2D* a) {
    for (auto it = affectors.begin(); it != affectors.end(); ++it) {
        if (static_cast<CameraAffector2D*>(it->get()) == a) {
            affectors.erase(it);
            break;
        }
    }
}

void Camera2D::refreshProjMatrix(glm::mat4& proj, const VkViewport& viewport) {
    proj = glm::ortho(viewport.x,
                      viewport.x + viewport.width,
                      viewport.y + viewport.height,
                      viewport.y,
                      nearPlane(),
                      farPlane());
}

void Camera2D::refreshViewMatrix(glm::mat4& view) {
    glm::vec2 c = center;
    glm::vec2 s = size;
    float r     = rotation;
    for (auto& affector : affectors) { affector->applyOnView(c, s, r); }

    const glm::vec2 offset = c - s * 0.5f;
    view                   = glm::rotate(r, Config::Rotate2DAxis);
    view                   = glm::translate(view, glm::vec3{-offset, 0.f});
}

void Camera2D::update(float dt) {
    if (controller) { controller->update(dt); }
    bool view = false;
    for (auto& a : affectors) { a->update(dt, view); }
    if (view) { markViewDirty(); }
}

} // namespace c2d
} // namespace render
} // namespace bl
