#include <BLIB/Cameras/2D/Camera2D.hpp>

#include <BLIB/Render/Config.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace bl
{
namespace cam
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

void Camera2D::refreshProjMatrix(glm::mat4& proj, const VkViewport&) {
    glm::vec2 c = center;
    glm::vec2 s = size;
    for (auto& a : affectors) { a->applyOnProj(c, s); }

    proj = glm::ortho(c.x - s.x * 0.5f,
                      c.x + s.x * 0.5f,
                      c.y - s.y * 0.5f,
                      c.y + s.y * 0.5f,
                      nearPlane(),
                      farPlane());
}

void Camera2D::refreshViewMatrix(glm::mat4& view) {
    float r = rotation;
    for (auto& affector : affectors) { affector->applyOnView(r); }

    view = glm::translate(glm::vec3(center, 0.f));
    view = glm::rotate(view, glm::radians(r), rc::Config::Rotate2DAxis);
    view = glm::translate(view, glm::vec3(-center, 0.f));
}

void Camera2D::update(float dt) {
    if (controller) { controller->update(dt); }
    bool view = false;
    bool proj = false;
    for (auto& a : affectors) { a->update(dt, view, proj); }
    if (view) { markViewDirty(); }
    if (proj) { markProjDirty(); }
}

} // namespace cam
} // namespace bl
