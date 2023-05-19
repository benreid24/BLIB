#ifndef BLIB_RENDER_CAMERAS_2D_CAMERA2D_HPP
#define BLIB_RENDER_CAMERAS_2D_CAMERA2D_HPP

#include <BLIB/Render/Cameras/2D/CameraAffector2D.hpp>
#include <BLIB/Render/Cameras/2D/CameraController2D.hpp>
#include <BLIB/Render/Cameras/Camera.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace bl
{
namespace render
{
/// Collection of classes for 2d cameras
namespace c2d
{
/**
 * @brief Camera for 2d scenes
 *
 * @ingroup Renderer
 */
class Camera2D : public Camera {
public:
    /**
     * @brief Creates a new 2d camera
     *
     * @param center The center of the camera's view
     * @param size The size of the visible area
     * @param rotation The amount of rotation around the view in degrees
     */
    Camera2D(const glm::vec2& center, const glm::vec2& size, float rotation = 0.f);

    /**
     * @brief Creates a new 2d camera
     *
     * @param area The visible area
     * @param rotation The amount of rotation around the view in degrees
     */
    Camera2D(const sf::FloatRect& area, float rotation = 0.f);

    /**
     * @brief Destroys the camera
     */
    virtual ~Camera2D() = default;

    /**
     * @brief Sets the center of the camera view
     *
     * @param center The new center of the camera view
     */
    void setCenter(const glm::vec2& center);

    /**
     * @brief Applies the given offset to the camera's center
     *
     * @param offset The amount to add to the camera's center position
     */
    void move(const glm::vec2& offset);

    /**
     * @brief Returns the camera's center position
     */
    constexpr const glm::vec2& getCenter() const;

    /**
     * @brief Sets the size of the visible area
     *
     * @param size The size of the visible area
     */
    void setSize(const glm::vec2& size);

    /**
     * @brief Multiplies the size of the visible area by the given scalar
     *
     * @param factor The amount to scale the visible area size by
     */
    void zoom(float factor);

    /**
     * @brief Multiplies the size of the visible area by the given scale factors
     *
     * @param factors The scale factors to multiple the visible area size by
     */
    void zoom(const glm::vec2& factors);

    /**
     * @brief Adds the given amount to the visible area size
     *
     * @param delta The amount to add to the visible area size
     */
    void changeSize(const glm::vec2& delta);

    /**
     * @brief Returns the size of the visible area
     */
    constexpr const glm::vec2& getSize() const;

    /**
     * @brief Sets the top left corner of the camera's visible area
     *
     * @param corner The new top left corner
     */
    void setCorner(const glm::vec2& corner);

    /**
     * @brief Returns the top left corner of the camera's visible area
     */
    glm::vec2 getCorner() const;

    /**
     * @brief Sets the visible area of the camera
     *
     * @param area The visible area of the camera
     */
    void setVisibleArea(const sf::FloatRect& area);

    /**
     * @brief Returns the visible area of the camera
     */
    sf::FloatRect getVisibleArea() const;

    /**
     * @brief Sets the rotation of the transform
     *
     * @param angle The angle in degrees
     */
    void setRotation(float angle);

    /**
     * @brief Applies the given rotation to the current rotation
     *
     * @param delta Amount of degrees to add to the current rotation
     */
    void rotate(float delta);

    /**
     * @brief Returns the current rotation in degrees
     */
    constexpr float getRotation() const;

    /**
     * @brief Sets the controller of this camera, freeing the previous controller, if any
     *
     * @tparam TController The type of controller to create. Must inherit from CameraController3D
     * @tparam ...TArgs Argument types to the controller constructor
     * @param ...args Arguments to the controller constructor
     * @return Pointer to the newly created controller
     */
    template<typename TController, typename... TArgs>
    TController* setController(TArgs&&... args);

    /**
     * @brief Sets the controller of this camera, freeing the previous controller, if any
     *
     * @tparam TController The type of controller to add
     * @param controller Pointer to the controller to add
     * @return Pointer to the controller
     */
    template<typename TController>
    TController* setController(std::unique_ptr<TController>&& controller);

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
    void removeAffector(const CameraAffector2D* affector);

private:
    glm::vec2 center;
    glm::vec2 size;
    float rotation;

    std::unique_ptr<CameraController2D> controller;
    std::vector<std::unique_ptr<CameraAffector2D>> affectors;

    virtual void refreshProjMatrix(glm::mat4& proj, const VkViewport& viewport) override;
    virtual void refreshViewMatrix(glm::mat4& view) override;
    virtual void update(float dt) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const glm::vec2& Camera2D::getCenter() const { return center; }

inline constexpr const glm::vec2& Camera2D::getSize() const { return size; }

inline constexpr float Camera2D::getRotation() const { return rotation; }

template<typename TController, typename... TArgs>
TController* Camera2D::setController(TArgs&&... args) {
    static_assert(std::is_base_of<CameraController2D, TController>::value,
                  "Controller must inherit from CameraController2D");

    TController* c = new TController(std::forward<TArgs>(args)...);
    controller.reset(c);
    controller->setCam(*this);
    return c;
}

template<typename TController>
TController* Camera2D::setController(std::unique_ptr<TController>&& c) {
    static_assert(std::is_base_of<CameraController2D, TController>::value,
                  "Controller must inherit from CameraController2D");

    TController* cr = c.get();
    controller.release();
    controller.swap(c);
    controller->setCam(*this);
    return c;
}

template<typename TAffector, typename... TArgs>
TAffector* Camera2D::addAffector(TArgs&&... args) {
    static_assert(std::is_base_of<CameraAffector2D, TAffector>::value,
                  "Affector must inherit from CameraAffector2D");

    TAffector* a = new TAffector(std::forward<TArgs>(args)...);
    affectors.emplace_back(a);
    return a;
}

template<typename TAffector>
TAffector* Camera2D::getAffector() const {
    static_assert(std::is_base_of<CameraAffector2D, TAffector>::value,
                  "Affector must inherit from CameraAffector2D");

    for (auto& a : affectors) {
        TAffector* af = dynamic_cast<TAffector*>(a.get());
        if (af) { return af; }
    }
    return nullptr;
}

template<typename TAffector>
void Camera2D::removeAffector() {
    static_assert(std::is_base_of<CameraAffector2D, TAffector>::value,
                  "Affector must inherit from CameraAffector2D");

    for (auto it = affectors.begin(); it != affectors.end(); ++it) {
        TAffector* a = dynamic_cast<TAffector*>(it->get());
        if (a) {
            affectors.erase(it);
            break;
        }
    }
}

template<typename TAffector>
void Camera2D::removeAffectors() {
    static_assert(std::is_base_of<CameraAffector2D, TAffector>::value,
                  "Affector must inherit from CameraAffector2D");

    for (int i = affectors.size(); i >= 0; --i) {
        TAffector* a = dynamic_cast<TAffector*>(affectors[i].get());
        if (a) { affectors.erase(affectors.begin() + i); }
    }
}

} // namespace c2d
} // namespace render
} // namespace bl

#endif
