#ifndef BLIB_MODELS_BONESET_HPP
#define BLIB_MODELS_BONESET_HPP

#include <BLIB/Models/Bone.hpp>
#include <assimp/scene.h>
#include <optional>
#include <vector>

namespace bl
{
namespace mdl
{
/**
 * @brief The set of bones in a model
 *
 * @ingroup Models
 */
class BoneSet {
public:
    /**
     * @brief Creates an empty bone set
     */
    BoneSet() = default;

    /**
     * @brief Creates the bone set and tries to load all bones
     *
     * @param scene The scene to load bones from
     */
    void populate(const aiScene* scene);

    /**
     * @brief Returns the index of the bone in the set. If the bone is not in the set it is added
     *
     * @param bone The bone to get or add
     * @return The index of the bone
     */
    unsigned int getOrAddBone(const aiBone* bone);

    /**
     * @brief Finds the bone with the given name and returns its index if found
     *
     * @param name The name of the bone to find
     * @return The index of the bone
     */
    std::optional<unsigned int> getBoneIndexByName(const std::string_view& name) const;

    /**
     * @brief Returns the bone at the given index
     *
     * @param index The index of the bone to fetch
     * @return The bone at the given index
     */
    const Bone& getBone(unsigned int index) const { return bones[index]; }

    /**
     * @brief Returns the number of bones in the set
     */
    unsigned int numBones() const { return bones.size(); }

private:
    std::vector<Bone> bones;
};

} // namespace mdl
} // namespace bl

#endif
