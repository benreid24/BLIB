#ifndef BLIB_RENDER_LIGHTING_LIGHT2D_HPP
#define BLIB_RENDER_LIGHTING_LIGHT2D_HPP

#include <cstdint>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace lgt
{
class Scene2DLighting;

/**
 * @brief Handle for a light in a 2d scene
 *
 * @ingroup Renderer
 */
class Light2D {
public:
    /**
     * @brief Creates an invalid light handle
     */
    Light2D();

    /**
     * @brief Removes the light from the scene. Invalidates the handle and any other handles to the
     *        same light. Lights are not removed by the handle's destructor.
     */
    void removeFromScene();

    /**
     * @brief Sets the color of the light. Light color is multiplied by fragment color
     *
     * @param color The color of the light
     */
    void setColor(const glm::vec3& color);

    /**
     * @brief Returns the color of the light
     */
    glm::vec3 getColor() const;

    /**
     * @brief Sets the radius of the light. The color fades to 0 at the radius
     *
     * @param radius The radius of the light in world coordinates
     */
    void setRadius(float radius);

    /**
     * @brief Returns the radius of the light in world coordinates
     */
    float getRadius() const;

    /**
     * @brief Sets the position of the light in world coordinates
     *
     * @param position The position of the light in world coordinates
     */
    void setPosition(const glm::vec2& position);

    /**
     * @brief Returns the position of the light in world coordinates
     */
    glm::vec2 getPosition() const;

    /**
     * @brief Returns whether or not this handle references a valid light
     */
    bool isValid() const;

    /**
     * @brief Returns the unique id of this light handle
     */
    std::uint32_t getId() const { return index; }

private:
    Scene2DLighting* owner;
    std::uint32_t index;

    Light2D(Scene2DLighting& owner, std::uint32_t i);

    friend class Scene2DLighting;
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
