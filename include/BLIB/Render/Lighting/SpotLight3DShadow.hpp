#ifndef BLIB_RENDER_LIGHTING_SPOTLIGHT3DSHADOW_HPP
#define BLIB_RENDER_LIGHTING_SPOTLIGHT3DSHADOW_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Lighting/SpotLight3D.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace bl
{
namespace rc
{
namespace lgt
{
/**
 * @brief 3D spotlight that casts a shadow
 *
 * @ingroup Renderer
 */
class SpotLight3DShadow : public SpotLight3D {
public:
    /**
     * @brief Initializes the spotlight with sane defaults
     */
    SpotLight3DShadow() = default;

    /**
     * @brief Computes the new view projection matrix for the light
     */
    void updateLightMatrix() {
        glm::mat4 projection =
            glm::perspective(outerCutoff * 2.f, 1.f, 0.1f, computeFalloffRadius());
        glm::mat4 view       = glm::lookAt(pos, pos + dir, Config::UpDirection);
        viewProjectionMatrix = projection * view;
    }

    /**
     * @brief Copies the light data from another light to this one which will be used by shaders
     *
     * @param other The light to copy from
     */
    void copyAsUniform(const SpotLight3DShadow& other) {
        SpotLight3D::copyAsUniform(other);
        updateLightMatrix();
    }

    /**
     * @brief Returns the byte offset of the camera matrix
     */
    static constexpr std::uint32_t getMatrixOffset() {
        return offsetof(SpotLight3DShadow, viewProjectionMatrix);
    }

private:
    glm::mat4 viewProjectionMatrix;

    friend class Scene3DLighting;
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
