#ifndef BLIB_CAMERAS_3D_CAMERA3D_HPP
#define BLIB_CAMERAS_3D_CAMERA3D_HPP

#include <BLIB/Cameras/3D/CameraAffector3D.hpp>
#include <BLIB/Cameras/3D/CameraController3D.hpp>
#include <BLIB/Cameras/Camera.hpp>
#include <BLIB/Components/Orientation3D.hpp>
#include <memory>
#include <type_traits>
#include <vector>

namespace bl
{
namespace cam
{
/**
 * @brief Camera for 3d scenes
 *
 * @ingroup Cameras
 */
class Camera3D : public Camera {
public:
    /**
     * @brief Create a new Camera
     *
     * @param pos The position of the camera in world space
     * @param yaw The yaw angle of the camera in degrees
     * @param pitch The pitch angle of the camera in degrees
     * @param fov The field of view of the camera
     */
    Camera3D(const glm::vec3& pos = {}, float yaw = 0.f, float pitch = 0.f, float fov = 75.f,
             float roll = 0.f);

    /**
     * @brief Creates a new camera
     *
     * @param pos The position of the camera
     * @param lookAt The position the camera should look at
     * @param fov The field of view of the camera
     * @param roll The roll of the camera
     */
    Camera3D(const glm::vec3& pos, const glm::vec3& lookAt, float fov = 75.f, float roll = 0.f);

    /**
     * @brief Destroys the camera
     */
    virtual ~Camera3D() = default;

    /**
     * @brief Set the world position of the camera
     *
     * @param pos The position of the camera in world space
     */
    void setPosition(const glm::vec3& pos);

    /**
     * @brief Applies an offset to the camera's position
     *
     * @param offset World space units to add to the position
     */
    void move(const glm::vec3& offset);

    /**
     * @brief Returns the position of the camera in world space
     */
    const glm::vec3& getPosition() const;

    /**
     * @brief Returns the orientation of the camera
     */
    const com::Orientation3D& getOrientation() const;

    /**
     * @brief Returns the orientation of the camera and marks the camera as needing to refresh the
     *        view matrix. Use this method to change the orientation of the camera
     */
    com::Orientation3D& getOrientationForChange();

    /**
     * @brief Set the FOV of the camera
     *
     * @param fov The FOV of the camera in degrees
     */
    void setFov(float fov);

    /**
     * @brief Returns the FOV of the camera
     */
    float getFov() const;

    /**
     * @brief Pushes a new controller onto the camera, superseceding the previous
     *
     * @tparam TController The type of controller to create. Must inherit from CameraController3D
     * @tparam ...TArgs Argument types to the controller constructor
     * @param ...args Arguments to the controller constructor
     * @return Pointer to the newly created controller
     */
    template<typename TController, typename... TArgs>
    TController* pushController(TArgs&&... args);

    /**
     * @brief Pushes a new controller onto the camera, superseceding the previous
     *
     * @tparam TController The type of controller to add
     * @param controller Pointer to the controller to add
     * @return Pointer to the controller
     */
    template<typename TController>
    TController* pushController(std::unique_ptr<TController>&& controller);

    /**
     * @brief Pops the current controller, making the prior controller active again
     */
    void popController();

    /**
     * @brief Removes the specific controller from the controller stack
     *
     * @param controller The camera controller to remove
     */
    void removeController(const CameraController3D* controller);

    /**
     * @brief Removes all controllers of the given type from the controller stack
     *
     * @tparam TController The type of controller to remove
     */
    template<typename TController>
    void removeController();

    /**
     * @brief Clears the entire controller stack
     */
    void removeAllControllers();

    /**
     * @brief Creates and adds an affector to the camera
     *
     * @tparam TAffector The type of affector to create
     * @tparam ...TArgs Argument types to the affector's constructor
     * @param ...args Arguments to the affector's constructor
     * @return A pointer to the new affector
     */
    template<typename TAffector, typename... TArgs>
    TAffector* addAffector(TArgs&&... args);

    /**
     * @brief Helper function to fetch a particular affector on this camera
     *
     * @tparam TAffector The type of affector to get
     * @return A pointer to the first affector of the given type, nullptr if not found
     */
    template<typename TAffector>
    TAffector* getAffector() const;

    /**
     * @brief Removes the first affector found of the given type from this camera
     *
     * @tparam TAffector The type of affector to remove
     */
    template<typename TAffector>
    void removeAffector();

    /**
     * @brief Removes all affectors of the given type from the camera
     *
     * @tparam TAffector The type of affectors to remove
     */
    template<typename TAffector>
    void removeAffectors();

    /**
     * @brief Removes the given affector from this camera
     *
     * @param affector Pointer to the affector to remove
     */
    void removeAffector(const CameraAffector3D* affector);

    /**
     * @brief Returns the size of the viewport
     */
    virtual glm::vec2 getViewerSize() const override;

    /**
     * @brief Returns the position that the camera is observing from
     */
    virtual glm::vec3 getObserverPosition() const override;

private:
    glm::vec3 position;
    com::Orientation3D orientation;
    float fov;
    glm::vec2 cachedViewportSize;

    std::vector<std::unique_ptr<CameraController3D>> controllers;
    std::vector<std::unique_ptr<CameraAffector3D>> affectors;

    virtual void refreshProjMatrix(glm::mat4& proj, const VkViewport& viewport) override;
    virtual void refreshViewMatrix(glm::mat4& view) override;
    virtual void update(float dt) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline const glm::vec3& Camera3D::getPosition() const { return position; }

inline float Camera3D::getFov() const { return fov; }

template<typename TController, typename... TArgs>
TController* Camera3D::pushController(TArgs&&... args) {
    static_assert(std::is_base_of<CameraController3D, TController>::value,
                  "Controller must inherit from CameraController3D");

    TController* ptr = new TController(std::forward<TArgs>(args)...);
    auto& c          = controllers.emplace_back(ptr);
    c->setCam(*this);
    return ptr;
}

template<typename TController>
TController* Camera3D::pushController(std::unique_ptr<TController>&& c) {
    static_assert(std::is_base_of<CameraController3D, TController>::value,
                  "Controller must inherit from CameraController3D");

    TController* ptr = c.get();
    auto& ca         = controllers.emplace_back(std::move(c));
    ca->setCam(*this);
    return ptr;
}

inline const com::Orientation3D& Camera3D::getOrientation() const { return orientation; }

template<typename TController>
void Camera3D::removeController() {
    std::erase_if(controllers, [](const std::unique_ptr<CameraController3D>& c) {
        return dynamic_cast<TController*>(c.get()) != nullptr;
    });
}

template<typename TAffector, typename... TArgs>
TAffector* Camera3D::addAffector(TArgs&&... args) {
    static_assert(std::is_base_of<CameraAffector3D, TAffector>::value,
                  "Affector must inherit from CameraAffector3D");

    TAffector* a = new TAffector(std::forward<TArgs>(args)...);
    affectors.emplace_back(a);
    return a;
}

template<typename TAffector>
TAffector* Camera3D::getAffector() const {
    static_assert(std::is_base_of<CameraAffector3D, TAffector>::value,
                  "Affector must inherit from CameraAffector3D");

    for (auto& a : affectors) {
        TAffector* af = dynamic_cast<TAffector*>(a.get());
        if (af) { return af; }
    }
    return nullptr;
}

template<typename TAffector>
void Camera3D::removeAffector() {
    static_assert(std::is_base_of<CameraAffector3D, TAffector>::value,
                  "Affector must inherit from CameraAffector3D");

    for (auto it = affectors.begin(); it != affectors.end(); ++it) {
        TAffector* a = dynamic_cast<TAffector*>(it->get());
        if (a) {
            affectors.erase(it);
            break;
        }
    }
}

template<typename TAffector>
void Camera3D::removeAffectors() {
    static_assert(std::is_base_of<CameraAffector3D, TAffector>::value,
                  "Affector must inherit from CameraAffector3D");

    for (int i = affectors.size(); i >= 0; --i) {
        TAffector* a = dynamic_cast<TAffector*>(affectors[i].get());
        if (a) { affectors.erase(affectors.begin() + i); }
    }
}

} // namespace cam
} // namespace bl

#endif
