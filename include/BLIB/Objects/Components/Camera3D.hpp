#ifndef BLIB_OBJECTS_COMPONENTS_CAMERA3D_HPP
#define BLIB_OBJECTS_COMPONENTS_CAMERA3D_HPP

#include <BLIB/Components/Camera3D.hpp>
#include <BLIB/ECS/EntityBacked.hpp>

namespace bl
{
namespace obj
{
namespace bcom
{
/**
 * @brief Base class for objects that can have 3d cameras attached to them
 *
 * @ingroup Objects
 */
class Camera3D : protected virtual ecs::EntityBacked {
public:
    /**
     * @brief Does nothing
     */
    Camera3D();

    /**
     * @brief Does nothing
     */
    virtual ~Camera3D() = default;

    /**
     * @brief Creates and attaches a 3d camera component to the entity of this object
     *
     * @param offset The local offset of the camera from the position of the object
     * @param localFaceDir The local face direction of the camera from the object
     * @param fov The field of view of the camera in degrees
     * @param roll The roll angle of the camera in degrees
     */
    void attachCamera(const glm::vec3& offset       = {},
                      const glm::vec3& localFaceDir = glm::vec3(0.f, 0.f, -1.f), float fov = 90.f,
                      float roll = 0.f);

    /**
     * @brief Removes the camera component from the object
     */
    void removeCamera();

    /**
     * @brief Makes the camera active for the given player
     *
     * @param player The player to activate the camera on
     */
    void makeCameraActive(unsigned int player = 0);

    /**
     * @brief Makes the camera inactive for the given player
     *
     * @param player The player to deactivate the camera on
     */
    void makeCameraInactive(unsigned int player = 0);

    /**
     * @brief Returns the camera component of this object if there is one
     */
    com::Camera3D* getCamera() const { return camera; }

private:
    com::Camera3D* camera;
};

} // namespace bcom
} // namespace obj
} // namespace bl

#endif
