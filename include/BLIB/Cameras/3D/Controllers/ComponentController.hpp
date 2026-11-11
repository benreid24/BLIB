#ifndef BLIB_CAMERAS_3D_CONTROLLERS_COMPONENTCONTROLLER_HPP
#define BLIB_CAMERAS_3D_CONTROLLERS_COMPONENTCONTROLLER_HPP

#include <BLIB/Cameras/3D/CameraController3D.hpp>
#include <BLIB/Components/Camera3D.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/Signals/Listener.hpp>

namespace bl
{
namespace cam
{
namespace c3d
{
/**
 * @brief Camera controller that pulls camera transform and settings from ECS components. Added
 *        automatically when a com::Camera3D is made active
 *
 * @ingroup Cameras
 */
class ComponentController
: public CameraController3D
, public sig::Listener<ecs::event::ComponentRemoved<com::Camera3D>> {
public:
    /**
     * @brief Creates the camera controller
     *
     * @param engine The game engine instance
     */
    ComponentController(engine::Engine& engine);

    /**
     * @brief Destroys the camera controller
     */
    virtual ~ComponentController() = default;

private:
    std::vector<com::Camera3D*> cameras;

    void update(float dt) override;
    void process(const ecs::event::ComponentRemoved<com::Camera3D>& signal) override;
    void removeCamera(const com::Camera3D& cam);

    friend class com::Camera3D;
};

} // namespace c3d
} // namespace cam
} // namespace bl

#endif
