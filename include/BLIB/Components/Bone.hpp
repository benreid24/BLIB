#ifndef BLIB_COMPONENTS_BONE_HPP
#define BLIB_COMPONENTS_BONE_HPP

#include <glm/glm.hpp>

namespace bl
{
namespace com
{
class Transform3D;

/**
 * @brief Component representing a bone in a skeletal model
 *
 * @ingroup Components
 */
struct Bone {
    glm::mat4 nodeBindPoseLocal;
    glm::mat4 boneOffset;
    com::Transform3D* transform;
    std::uint32_t boneIndex;

    /**
     * @brief Creates a default bone
     */
    Bone()
    : nodeBindPoseLocal(1.f)
    , transform(nullptr)
    , boneIndex(0)
    , boneOffset(1.f) {}
};

} // namespace com
} // namespace bl

#endif
