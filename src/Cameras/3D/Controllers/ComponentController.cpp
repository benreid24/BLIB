#include <BLIB/Cameras/3D/Controllers/ComponentController.hpp>

#include <BLIB/Cameras/3D/Camera3D.hpp>
#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace cam
{
namespace c3d
{
ComponentController::ComponentController(engine::Engine& engine) {
    subscribe(engine.ecs().getSignalChannel());
}

void ComponentController::update(float) {
    if (!cameras.empty()) {
        auto& cam = *cameras.back();
        camera().setPosition(cam.getTransform().getGlobalPosition());
        camera().getOrientationForChange().setRotationQuaternion(
            cam.getTransform().getGlobalRotation());
        camera().setFov(cam.getFOV());
    }
}

void ComponentController::process(const ecs::event::ComponentRemoved<com::Camera3D>& signal) {
    removeCamera(signal.component);
}

void ComponentController::removeCamera(const com::Camera3D& cam) {
    std::erase_if(cameras, [&cam](com::Camera3D* c) { return c == &cam; });
}

} // namespace c3d
} // namespace cam
} // namespace bl
