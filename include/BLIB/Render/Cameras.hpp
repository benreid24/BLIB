#ifndef BLIB_RENDER_CAMERAS_HPP
#define BLIB_RENDER_CAMERAS_HPP

#include <BLIB/Render/Cameras/Camera.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <stack>

/**
 * @addtogroup Cameras
 * @ingroup RenderSystem
 * @brief Collection of cameras and utilities for controlling what is rendered
 *
 */

namespace bl
{
/// @brief Collection of classes comprising the game engine's rendering system
namespace render
{
/**
 * @brief Manages the currently active camera and the stack of previously active cameras
 *
 * @ingroup RenderSystem
 * @ingroup Cameras
 *
 */
class Cameras {
public:
    /**
     * @brief Construct a new Cameras system
     * 
     */
    Cameras();

    /**
     * @brief Returns the currently active camera
     *
     */
    camera::Camera::Ptr activeCamera();

    /**
     * @brief Adds a new camera to immediately use, saving the previous active camera
     *
     * @param camera The new camera to use
     */
    void pushCamera(const camera::Camera::Ptr& camera);

    /**
     * @brief Replaces the current active camera with a new one. The previous camera is not able to
     *        be returned to
     *
     * @param camera The new camera to use
     */
    void replaceCamera(const camera::Camera::Ptr& camera);

    /**
     * @brief Removes the current active camera and resets to the previous one
     *
     */
    void popCamera();

    /**
     * @brief Clears the current camera stack and pushes the given camera to it
     *
     * @param camera The camera to replace all current cameras with
     */
    void clearAndReplace(const camera::Camera::Ptr& camera);

    /**
     * @brief Sets the region of world coordinates to limit rendering to. Use this to avoid
     *        rendering regions outside of maps as best as possible.
     *
     * @param worldArea The region, in world coordinates, to constrain rendering to
     */
    void setViewportConstraint(const sf::FloatRect& worldArea);

    /**
     * @brief Removes the viewport rendering constraint
     * 
     */
    void clearViewportConstraint();

    /**
     * @brief Modifies the given view to have the proper size and position. Zoom is applied as a
     *        factor to the existing view size. No change is made to the viewport
     *
     * @param target The target to configure the view on
     *
     */
    void configureView(sf::RenderTarget& target) const;

    /**
     * @brief Returns the area to be rendered by the active camera
     *
     */
    const sf::FloatRect& getCurrentViewport() const;

    /**
     * @brief Updates the current active camera. Pops invalidated cameras as necessary
     *
     * @param dt Time elapsed in seconds since last call to update
     */
    void update(float dt);

private:
    std::stack<camera::Camera::Ptr> cameras;
    sf::FloatRect renderRegion;

    bool clearInvalid();
};

} // namespace render
} // namespace bl

#endif
