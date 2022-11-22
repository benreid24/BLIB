#ifndef BLIB_RENDER_CAMERAS_STATICCAMERA_HPP
#define BLIB_RENDER_CAMERAS_STATICCAMERA_HPP

#include <BLIB/Render/Cameras/Camera.hpp>

namespace bl
{
namespace render
{
namespace camera
{
/**
 * @brief Basic camera that always renders a constant viewport
 *
 */
class StaticCamera : public Camera {
public:
    /// @brief A pointer to a StaticCamera
    using Ptr = std::shared_ptr<StaticCamera>;

    /**
     * @brief Construct a new Static Camera from a size. Corner is (0, 0)
     *
     * @param size The size of the viewport to render
     * @param rotation The camera rotation in degrees
     * @return Ptr The new camera
     */
    static Ptr create(const sf::Vector2f& size, float rotation = 0.f);

    /**
     * @brief Construct a new Static Camera
     *
     * @param viewport The viewport to always render
     * @param rotation The camera rotation in degrees
     * @return Ptr The new camera
     */
    static Ptr create(const sf::FloatRect& viewport, float rotation = 0.f);

    /**
     * @brief Does nothing
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
     * @brief Construct a new Static Camera
     *
     * @param viewport The viewport to always render
     * @param rotation The camera rotation in degrees
     */
    StaticCamera(const sf::FloatRect& viewport, float rotation = 0.f);
};

} // namespace camera
} // namespace render
} // namespace bl

#endif
