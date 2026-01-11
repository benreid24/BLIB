#ifndef BLIB_RENDER_LIGHTING_COLOR3D_HPP
#define BLIB_RENDER_LIGHTING_COLOR3D_HPP

#include <BLIB/Render/Color.hpp>
#include <BLIB/Render/Config/Constants.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace lgt
{
/**
 * @brief Basic struct containing the color of a 3d light
 *
 * @ingroup Renderer
 */
class Color3D {
public:
    /**
     * @brief Initializes the light to white with default factors
     */
    Color3D() { setLighting(glm::vec3(1.f, 1.f, 1.f)); }

    /**
     * @brief Sets the color of the light using a single color and a set of factors
     *
     * @param color The normalized color of the light
     * @param brightness A linear factor to apply to the light color
     * @param specularFactor The factor to use to compute the specular color
     * @param diffuseFactor The factor to use to compute the diffuse color
     * @param ambientFactor The factor to use to compute the ambient color
     */
    void setLighting(const glm::vec3& color, float brightness = 1.f,
                     float specularFactor = cfg::Constants::DefaultSpecularLightFactor,
                     float diffuseFactor  = cfg::Constants::DefaultDiffuseLightFactor,
                     float ambientFactor  = cfg::Constants::DefaultAmbientLightFactor) {
        ambient  = glm::vec4(color * ambientFactor * brightness, brightness);
        diffuse  = glm::vec4(color * diffuseFactor * brightness, specularFactor);
        specular = color * specularFactor * brightness;
    }

    /**
     * @brief Sets the base color of the light. Will be adjusted by brightness and factors
     *
     * @param color The base color of the light
     */
    void setColor(const glm::vec3& color) {
        setLighting(color, ambient.w, diffuse.w, getDiffuseFactor(), getAmbientFactor());
    }

    /**
     * @brief Sets the linear brightness factor of the light
     *
     * @param brightness The factor to multiply the colors by
     */
    void setBrightness(float brightness) {
        setLighting(getColor(), brightness, diffuse.w, getDiffuseFactor(), getAmbientFactor());
    }

    /**
     * @brief Sets the specular factor of the light
     *
     * @param factor The factor to multiply the specular color by
     */
    void setSpecularFactor(float factor) {
        setLighting(getColor(), ambient.w, factor, getDiffuseFactor(), getAmbientFactor());
    }

    /**
     * @brief Sets the diffuse factor of the light
     *
     * @param factor The factor to multiply the diffuse color by
     */
    void setDiffuseFactor(float factor) {
        setLighting(getColor(), ambient.w, diffuse.w, factor, getAmbientFactor());
    }

    /**
     * @brief Sets the ambient factor of the light
     *
     * @param factor The factor to multiply the ambient color by
     */
    void setAmbientFactor(float factor) {
        setLighting(getColor(), ambient.w, diffuse.w, getDiffuseFactor(), factor);
    }

    /**
     * @brief Returns the brightness factor of the light
     */
    float getBrightness() const { return ambient.w; }

    /**
     * @brief Returns the base color of the light before brightness is applied
     */
    glm::vec3 getColor() const { return specular / ambient.w / diffuse.w; }

    /**
     * @brief Returns the adjusted specular color of the light
     */
    const glm::vec3& getSpecularColor() const { return specular; }

    /**
     * @brief Returns the specular factor of the light
     */
    float getSpecularFactor() const { return diffuse.w; }

    /**
     * @brief Returns the adjusted diffuse color of the light
     */
    glm::vec3 getDiffuseColor() const { return diffuse; }

    /**
     * @brief Returns the diffuse factor of the light
     */
    float getDiffuseFactor() const { return diffuse.r / (getColor().r * ambient.w); }

    /**
     * @brief Returns the adjusted ambient color of the light
     */
    glm::vec3 getAmbientColor() const { return ambient; }

    /**
     * @brief Returns the ambient factor of the light
     */
    float getAmbientFactor() const { return ambient.r / (getColor().r * ambient.w); }

    /**
     * @brief Returns the maximum light level across all channels and light types
     */
    float getMaxLightLevel() const {
        return std::max({ambient.r,
                         ambient.g,
                         ambient.b,
                         diffuse.r,
                         diffuse.g,
                         diffuse.b,
                         specular.r,
                         specular.g,
                         specular.b});
    }

private:
    glm::vec4 ambient; // w = brightness
    glm::vec4 diffuse; // w = specularFactor
    alignas(16) glm::vec3 specular;
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
