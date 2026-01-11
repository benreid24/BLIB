#ifndef BLIB_MODELS_CONVERSIONHELPERS_HPP
#define BLIB_MODELS_CONVERSIONHELPERS_HPP

#include <assimp/color4.h>
#include <assimp/quaternion.h>
#include <assimp/types.h>
#include <assimp/vector2.h>
#include <assimp/vector3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace bl
{
namespace mdl
{
/**
 * @brief Collection of type conversion helpers for Assimp and glm types
 *
 * @ingroup Models
 */
struct Convert {
    /**
     * @brief Converts an Assimp 2d vector to a glm vector
     *
     * @param vec The Assimp vector
     * @return An equivalent glm vector
     */
    static glm::vec2 toVec2(const aiVector2D& vec) { return glm::vec2(vec.x, vec.y); }

    /**
     * @brief Converts an Assimp 2d vector to a glm vector
     *
     * @param vec The Assimp vector
     * @return An equivalent glm vector
     */
    static glm::vec2 toVec2(const aiVector3D& vec) { return glm::vec2(vec.x, vec.y); }

    /**
     * @brief Converts an Assimp 3d vector to a glm vector
     *
     * @param vec The Assimp vector
     * @return An equivalent glm vector
     */
    static glm::vec3 toVec3(const aiVector3D& vec) { return glm::vec3(vec.x, vec.y, vec.z); }

    /**
     * @brief Converts an Assimp 4d vector to a glm vector
     *
     * @param vec The Assimp vector
     * @return An equivalent glm vector
     */
    static glm::vec4 toVec4(const aiColor3D& col) { return glm::vec4(col.r, col.g, col.b, 1.f); }

    /**
     * @brief Converts an Assimp color to a glm vector
     *
     * @param vec The Assimp color
     * @return An equivalent glm vector
     */
    static glm::vec4 toVec4(const aiColor4D& col) { return glm::vec4(col.r, col.g, col.b, col.a); }

    /**
     * @brief Converts an Assimp color to a glm vector
     *
     * @param vec The Assimp color
     * @return An equivalent glm vector
     */
    static glm::quat toVec4(const aiQuaternion& quat) {
        return glm::quat(quat.w, quat.x, quat.y, quat.z);
    }

    /**
     * @brief Converts an Assimp matrix to a glm matrix
     *
     * @param mat The assimp matrix
     * @return An equivalent glm matrix
     */
    static glm::mat3 toMat3(const aiMatrix3x3& mat) {
        return glm::transpose(
            glm::mat3(mat.a1, mat.a2, mat.a3, mat.b1, mat.b2, mat.b3, mat.c1, mat.c2, mat.c3));
    }

    /**
     * @brief Converts an Assimp matrix to a glm matrix
     *
     * @param mat The assimp matrix
     * @return An equivalent glm matrix
     */
    static glm::mat4 toMat4(const aiMatrix4x4& mat) {
        return glm::transpose(glm::mat4(mat.a1,
                                        mat.a2,
                                        mat.a3,
                                        mat.a4,
                                        mat.b1,
                                        mat.b2,
                                        mat.b3,
                                        mat.b4,
                                        mat.c1,
                                        mat.c2,
                                        mat.c3,
                                        mat.c4,
                                        mat.d1,
                                        mat.d2,
                                        mat.d3,
                                        mat.d4));
    }
};

} // namespace mdl
} // namespace bl

#endif
