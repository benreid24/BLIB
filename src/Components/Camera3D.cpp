#include <BLIB/Components/Camera3D.hpp>

#include <BLIB/Cameras/3D/Camera3D.hpp>
#include <BLIB/Cameras/3D/Controllers/ComponentController.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Engine/Worlds/World3D.hpp>

namespace bl
{
namespace com
{
Camera3D::Camera3D(engine::Engine& engine, ecs::Entity entity, float fov)
: engine(engine)
, transform(engine.ecs().getComponent<Transform3D>(entity))
, fov(fov) {
    if (!transform) {
        BL_LOG_WARN << "Camera3D component created for entity " << entity.id
                    << " but no Transform3D component was found. Creating one";
        transform = engine.ecs().emplaceComponent<Transform3D>(entity);
    }

    localTransform.setParent(*transform);
}

void Camera3D::setFOV(float f) { fov = f; }

bool Camera3D::makeActive(unsigned int playerIndex) {
    auto& player       = engine.getPlayer(playerIndex);
    auto* camRaw       = player.getRenderObserver().getCurrentCamera();
    cam::Camera3D* cam = dynamic_cast<cam::Camera3D*>(camRaw);
    if (!cam) {
        if (camRaw) { return false; }

        engine::World& worldRaw = player.getCurrentWorld();
        engine::World3D* world  = dynamic_cast<engine::World3D*>(&worldRaw);
        if (!world) { return false; }

        player.getRenderObserver().setCamera<cam::Camera3D>();
    }

    cam::c3d::ComponentController* controller =
        cam->getCurrentController<cam::c3d::ComponentController>();
    if (!controller) { controller = cam->pushController<cam::c3d::ComponentController>(engine); }
    controller->cameras.push_back(this);

    return true;
}

void Camera3D::makeActiveForAllPlayers() {
    for (unsigned int i = 0; i < engine.playerCount(); ++i) { makeActive(i); }
}

bool Camera3D::makeInactive(unsigned int playerIndex) {
    auto& player       = engine.getPlayer(playerIndex);
    auto* camRaw       = player.getRenderObserver().getCurrentCamera();
    cam::Camera3D* cam = dynamic_cast<cam::Camera3D*>(camRaw);
    if (!cam) { return false; }

    std::vector<cam::c3d::ComponentController*> controllers =
        cam->getControllers<cam::c3d::ComponentController>();
    for (cam::c3d::ComponentController* controller : controllers) {
        controller->removeCamera(*this);
        if (controller->cameras.empty()) { cam->removeController(controller); }
    }
}

void Camera3D::makeInactiveForAllPlayers() {
    for (unsigned int i = 0; i < engine.playerCount(); ++i) { makeInactive(i); }
}

} // namespace com
} // namespace bl
