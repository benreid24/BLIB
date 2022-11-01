#include <BLIB/Render/Cameras.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace render
{
namespace
{
bool errorLogged = false;
}

Cameras::Cameras()
: renderRegion({-1.f, -1.f, -1.f, -1.f}) {}

camera::Camera::Ptr Cameras::activeCamera() { return cameras.top(); }

void Cameras::pushCamera(const camera::Camera::Ptr& cam) {
    cameras.emplace(cam);
    errorLogged = false;
}

void Cameras::replaceCamera(const camera::Camera::Ptr& cam) {
    if (!cameras.empty()) cameras.pop();
    errorLogged = false;
    cameras.emplace(cam);
}

void Cameras::popCamera() {
    if (!cameras.empty()) cameras.pop();
    errorLogged = false;
}

void Cameras::clearAndReplace(const camera::Camera::Ptr& cam) {
    while (!cameras.empty()) cameras.pop();
    cameras.emplace(cam);
    errorLogged = false;
}

void Cameras::setViewportConstraint(const sf::FloatRect& constraint) { renderRegion = constraint; }

void Cameras::clearViewportConstraint() { renderRegion.width = -1.f; }

const sf::FloatRect& Cameras::getCurrentViewport() const {
    static const sf::FloatRect fr;
    if (cameras.empty() || !cameras.top()->valid()) {
        BL_LOG_ERROR << "Accessing viewport when no cameras are active";
        return fr;
    }
    return cameras.top()->getViewport();
}

void Cameras::update(float dt) {
    if (!clearInvalid()) return;
    cameras.top()->update(dt);
}

void Cameras::configureView(sf::RenderTarget& target) const {
    if (cameras.empty()) {
        if (!errorLogged) {
            errorLogged = true;
            BL_LOG_ERROR << "Rendering with no active camera";
        }
        return;
    }
    if (!cameras.top()->valid()) {
        if (!errorLogged) {
            errorLogged = true;
            BL_LOG_ERROR << "Active camera was invalidated";
        }
        return;
    }

    // constrain the camera first
    camera::Camera& cam = const_cast<camera::Camera&>(*cameras.top());
    if (renderRegion.width > 0.f) {
        // camera is too big, just center what we have
        if (cam.viewport.width > renderRegion.width) {
            cam.viewport.left = renderRegion.left + renderRegion.width * 0.5f;
        }
        else if (cam.viewport.left < renderRegion.left) {
            cam.viewport.left = renderRegion.left;
        }
        else if (cam.viewport.left + cam.viewport.width > renderRegion.left + renderRegion.width) {
            cam.viewport.left = renderRegion.left + renderRegion.width - cam.viewport.width;
        }

        // camera is too tall, just center what we have
        if (cam.viewport.height > renderRegion.height) {
            cam.viewport.top = renderRegion.top + renderRegion.height * 0.5f;
        }
        else if (cam.viewport.top < renderRegion.top) {
            cam.viewport.top = renderRegion.top;
        }
        else if (cam.viewport.top + cam.viewport.height > renderRegion.top + renderRegion.height) {
            cam.viewport.top = renderRegion.top + renderRegion.height - cam.viewport.height;
        }
    }

    // apply the camera view
    sf::View view = target.getView();
    view.setCenter(cam.getCenter());
    view.setSize(cam.getSize());
    view.setRotation(cam.getRotation());
    target.setView(view);
}

bool Cameras::clearInvalid() {
    while (!cameras.empty() && !cameras.top()->valid()) { cameras.pop(); }
    return !cameras.empty();
}

} // namespace render
} // namespace bl
