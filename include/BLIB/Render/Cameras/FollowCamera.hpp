#ifndef BLIB_RENDER_CAMERAS_FOLLOWCAMERA_HPP
#define BLIB_RENDER_CAMERAS_FOLLOWCAMERA_HPP

#include <BLIB/Render/Cameras/Camera.hpp>

namespace bl
{
namespace render
{
namespace camera
{
/**
 * @brief Camera that maintains its center around an external position
 *
 * @ingroup Cameras
 *
 */
class FollowCamera : public Camera {
public:
    /// @brief Pointer to a FollowCamera
    using Ptr = std::shared_ptr<FollowCamera>;

    /**
     * @brief Construct a new Follow Camera
     *
     * @param follow The position to remain centered on. Must remain valid
     * @param size The size of the viewport around the position to follow
     * @return Ptr The new camera
     */
    static Ptr create(const sf::Vector2f* follow, const sf::Vector2f& size);

    /**
     * @brief Centers the view around the Vector2f being followed
     *
     */
    virtual void update(float) override;

    /**
     * @brief Always returns true
     *
     */
    virtual bool valid() const override;

protected:
    /**
     * @brief Construct a new Follow Camera
     *
     * @param follow The position to remain centered on. Must remain valid
     * @param size The size of the viewport around the position to follow
     */
    FollowCamera(const sf::Vector2f* follow, const sf::Vector2f& size);

private:
    const sf::Vector2f* follow;
};

} // namespace camera
} // namespace render
} // namespace bl

#endif
