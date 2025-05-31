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

    /**
     * @brief Creates the light with sane defaults
     */
    PointLight3DShadow() = default;

    /**
     * @brief Updates the view projection matrices for the light
     */
    void updateLightMatrices() {
        const glm::mat4 proj =
            glm::perspective(glm::radians(90.f), 1.f, 0.1f, computeFalloffRadius());
        for (unsigned int i = 0; i < 6; ++i) {
            viewProjectionMatrices[i] =
                proj * glm::lookAt(pos, pos + Config::CubemapDirections[i], Config::UpDirection);
        }
    }

    /**
     * @brief Copies the light data from another light to this one which will be used by shaders
     *
     * @param other The light to copy from
     */
    void copyAsUniform(const PointLight3DShadow& other) {
        PointLight3D::copyAsUniform(other);
        updateLightMatrices();
    }

    /**
     * @brief Returns the byte offset of the camera matrices
     */
    static constexpr std::uint32_t getMatrixOffset() {
        return offsetof(PointLight3DShadow, viewProjectionMatrices);
    }
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
