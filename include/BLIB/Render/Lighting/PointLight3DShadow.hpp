#ifndef BLIB_RENDER_LIGHTING_POINTLIGHT3DSHADOW_HPP
#define BLIB_RENDER_LIGHTING_POINTLIGHT3DSHADOW_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Lighting/PointLight3D.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace bl
{
namespace rc
{
namespace lgt
{
/**
 * @brief Omnidirectional light that casts shadows
 *
 * @ingroup Renderer
 */
struct PointLight3DShadow : public PointLight3D {
    glm::mat4 viewProjectionMatrices[6];
    alignas(8) float farPlane;
    alignas(8) float nearPlane;

    static constexpr std::array<glm::vec3, 6> UpDirs = {
        Config::UpDirection,
        Config::UpDirection,
        {-1.f, 0.f, 0.f},
        {0.f, 0.f, -1.f},
        Config::UpDirection,
        Config::UpDirection,
    };

    /**
     * @brief Creates the light with sane defaults
     */
    PointLight3DShadow()
    : farPlane(10.f)
    , nearPlane(0.1f) {
        for (unsigned int i = 0; i < 6; ++i) { viewProjectionMatrices[i] = glm::mat4(1.f); }
    }

    /**
     * @brief Updates the view projection matrices for the light
     */
    void updateLightMatrices() {
        farPlane             = computeFalloffRadius();
        const glm::mat4 proj = glm::perspective(glm::radians(90.f), 1.f, nearPlane, farPlane);
        for (unsigned int i = 0; i < 6; ++i) {
            viewProjectionMatrices[i] =
                proj * glm::lookAt(pos, pos + Config::CubemapDirections[i], UpDirs[i]);
        }
    }

    /**
     * @brief Copies the light data from another light to this one which will be used by shaders
     *
     * @param other The light to copy from
     */
    void copyAsUniform(const PointLight3DShadow& other) {
        PointLight3D::copyAsUniform(other);
        nearPlane = other.nearPlane;
        updateLightMatrices();
    }
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
