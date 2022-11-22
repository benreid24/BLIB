#ifndef BLIB_RENDER_CAMERAS_CAMERA_HPP
#define BLIB_RENDER_CAMERAS_CAMERA_HPP

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>

namespace bl
{
namespace render
{
class Cameras;

namespace camera
{
/**
 * @brief Base class for all cameras
 *
 * @ingroup Cameras
 *
 */
class Camera {
public:
    /// Pointer type to camera
    using Ptr = std::shared_ptr<Camera>;

    /**
     * @brief Destroy the Camera object
     *
     */
    virtual ~Camera() = default;

    /**
     * @brief Returns whether or not the camera is still valid
     *
     * @return True if the camera may still be used, false otherwise
     */
    virtual bool valid() const = 0;

    /**
     * @brief Updates the camera
     *
     * @param dt Time elapsed since last call to update
     */
    virtual void update(float dt) = 0;

    /**
     * @brief Returns the current viewport that this camera is rendering
     *
     * @return const sf::FloatRect& The viewport of this camera
     */
    const sf::FloatRect& getViewport() const;

    /**
     * @brief Returns the center of the camera
     *
     * @return sf::Vector2f The center of the region shown by the camera
     */
    sf::Vector2f getCenter() const;

    /**
     * @brief Returns the size of the camera
     *
     * @return sf::Vector2f The size of the area shwon by the camera
     */
    sf::Vector2f getSize() const;

    /**
     * @brief Returns the rotation of the camera
     *
     * @return float The rotation in degrees
     */
    float getRotation() const;

    /**
     * @brief Helper method to position the viewport by its center
     *
     * @param center The center of the camera view
     */
    void setCenter(const sf::Vector2f& center);

    /**
     * @brief Helper method to resize the viewport. Maintains the previous center
     *
     * @param center The new size of the viewport
     */
    void setSize(const sf::Vector2f& center);

    /**
     * @brief Multiples the current viewport size by the given factor
     *
     * @param factor The zoom factor
     */
    void zoom(float factor);

    /**
     * @brief Set the rotation of the camera
     *
     * @param degrees The rotation in degrees
     */
    void setRotation(float degrees);

    /**
     * @brief Rotates the camera
     *
     * @param degrees The amount of rotation to apply to the current rotation
     */
    void rotate(float degrees);

    /**
     * @brief Helper method to size the viewport using a zoom level and default size
     *
     * @param zoomLevel The zoom level to apply
     * @param baseSize The size of the viewport at a zoom level of 1.0
     */
    void setZoomLevel(float zoomLevel, const sf::Vector2f& baseSize);

    /**
     * @brief Set the viewport of the camera
     *
     * @param viewport The new viewport to render
     */
    void setViewport(const sf::FloatRect& viewport);

protected:
    /**
     * @brief Construct a new Camera object with sane defaults
     *
     */
    Camera();

    /**
     * @brief Creates the camera with the initial viewport
     *
     * @param viewport The initial viewport to use
     */
    Camera(const sf::FloatRect& viewport, float rotation);

private:
    sf::FloatRect viewport;
    float rotation;

    friend class ::bl::render::Cameras;
};

} // namespace camera
} // namespace render
} // namespace bl

#endif
