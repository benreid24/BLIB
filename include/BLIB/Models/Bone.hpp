#ifndef BLIB_MODELS_BONE_HPP
#define BLIB_MODELS_BONE_HPP

#include <BLIB/Models/ConversionHelpers.hpp>
#include <assimp/mesh.h>

namespace bl
{
namespace mdl
{
/**
 * @brief Basic struct representing a bone in a model
 *
 * @ingroup Models
 */
struct Bone {
    std::string name;
    glm::mat4 transform;

    /**
     * @brief Creates the bone from the given assimp bone
     *
     * @param bone The bone to create from
     */
    Bone(const aiBone* bone)
    : name(bone->mName.data, bone->mName.length)
    , transform(Convert::toMat4(bone->mOffsetMatrix)) {}
};

} // namespace mdl
} // namespace bl

#endif
