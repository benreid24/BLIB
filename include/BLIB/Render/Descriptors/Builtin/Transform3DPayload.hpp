#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_TRANSFORM3DPAYLOAD_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_TRANSFORM3DPAYLOAD_HPP

#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
/**
 * @brief Descriptor set payload for 3d transforms
 *
 * @ingroup Render
 */
struct Transform3DPayload {
    glm::mat4 transform;
    glm::mat3 normal;

    /**
     * @brief Creates the default payload
     */
    Transform3DPayload() = default;

    /**
     * @brief Creates the payload from the transform
     *
     * @param transform The transform matrix to use
     */
    Transform3DPayload(const glm::mat4& transform)
    : transform(transform) {
        computeNormal();
    }

    /**
     * @brief Updates the payload from the transform
     *
     * @param transform The transform matrix to use
     * @return A reference to this object
     */
    Transform3DPayload& operator=(const glm::mat4& transform) {
        this->transform = transform;
        computeNormal();
        return *this;
    }

    /**
     * @brief Computes the normal matrix from the transform matrix
     */
    void computeNormal() { normal = glm::transpose(glm::inverse(glm::mat3(transform))); }
};

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
