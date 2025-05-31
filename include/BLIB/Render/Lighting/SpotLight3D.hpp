#ifndef BLIB_RENDER_LIGHTING_SPOTLIGHT3D_HPP
#define BLIB_RENDER_LIGHTING_SPOTLIGHT3D_HPP

#include <BLIB/Render/Lighting/Attenuation.hpp>
#include <BLIB/Render/Lighting/Color3D.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace lgt
{
class Scene3DLighting;

/**
 * @brief Basic struct representing a spotlight in a 3d scene
 *
 * @ingroup Renderer
 */
class SpotLight3D {
public:
    /**
     * @brief Creates the light with sane defaults
     */
    SpotLight3D()
    : pos()
    , dir(0.f, -1.f, 0.f)
    , cutoff(1.f)
    , outerCutoff(20.f)
    , attenuation()
    , color() {}

    /**
     * @brief Returns the position of the light in world coordinates
     */
    const glm::vec3& getPosition() const { return pos; }

    /**
     * @brief Set the position of the light in world coordinates
     *
     * @param position The new position of the light
     */
    void setPosition(const glm::vec3& position) { pos = position; }

    /**
     * @brief Set the inner cutoff angle of the light in radians
     *
     * @param cutoffRadians The inner cutoff angle in radians
     */
    void setInnerCutoff(float cutoffRadians) { cutoff = cutoffRadians; }

    /**
     * @brief Set the inner cutoff angle of the light in degrees
     *
     * @param cutoffDegrees The inner cutoff angle in degrees
     */
    void setInnerCutoffDegrees(float cutoffDegrees) { cutoff = glm::radians(cutoffDegrees); }

    /**
     * @brief Get the inner cutoff angle of the light in radians
     */
    float getInnerCutoff() const { return cutoff; }

    /**
     * @brief Get the inner cutoff angle of the light in degrees
     */
    float getInnerCutoffDegrees() const { return glm::degrees(cutoff); }

    /**
     * @brief Set the outer cutoff angle of the light in degrees
     *
     * @param outerCutoffDegrees The outer cutoff angle in degrees
     */
    void setOuterCutoffDegrees(float outerCutoffDegrees) {
        outerCutoff = glm::radians(outerCutoffDegrees);
    }

    /**
     * @brief Set the outer cutoff angle of the light in radians
     *
     * @param outerCutoffRadians The outer cutoff angle in radians
     */
    void setOuterCutoff(float outerCutoffRadians) { outerCutoff = outerCutoffRadians; }

    /**
     * @brief Get the outer cutoff angle of the light in radians
     */
    float getOuterCutoff() const { return outerCutoff; }

    /**
     * @brief Get the outer cutoff angle of the light in degrees
     */
    float getOuterCutoffDegrees() const { return glm::degrees(outerCutoff); }

    /**
     * @brief Set the direction vector of the light
     *
     * @param direction The new direction vector of the light
     */
    void setDirection(const glm::vec3& direction) { dir = glm::normalize(direction); }

    /**
     * @brief Helper method to point the light at a specific position
     *
     * @param position The position to point at
     */
    void pointAt(const glm::vec3& position) { dir = glm::normalize(position - pos); }

    /**
     * @brief Returns the attenuation parameters of the light
     */
    const Attenuation& getAttenuation() const { return attenuation; }

    /**
     * @brief Returns the attenuation parameters of the light
     */
    Attenuation& getAttenuation() { return attenuation; }

    /**
     * @brief Returns the color of the light
     */
    const Color3D& getColor() const { return color; }

    /**
     * @brief Returns the color of the light
     */
    Color3D& getColor() { return color; }

    /**
     * @brief Returns the radius of the light volume for this light
     *
     * @param threshold The threshold to consider the bounds of the light volume
     * @return The radius of the light volume
     */
    float computeFalloffRadius(float threshold = 256.f / 5.f) const {
        return attenuation.computeFalloffRadius(threshold, color.getMaxLightLevel());
    }

    /**
     * @brief Copies the light data from another light to this one which will be used by shaders
     *
     * @param other The light to copy from
     */
    void copyAsUniform(const SpotLight3D& other) {
        pos         = other.pos;
        cutoff      = std::cos(other.cutoff);
        outerCutoff = std::cos(other.outerCutoff);
        dir         = other.dir;
        attenuation = other.attenuation;
        color       = other.color;
    }

protected:
    alignas(16) glm::vec3 pos;
    float cutoff;
    alignas(16) glm::vec3 dir;
    float outerCutoff;
    Attenuation attenuation;
    Color3D color;

    friend class Scene3DLighting;
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
