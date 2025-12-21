#ifndef BLIB_MODELS_ANIMATIONSET_HPP
#define BLIB_MODELS_ANIMATIONSET_HPP

#include <BLIB/Models/Animation.hpp>
#include <assimp/scene.h>

namespace bl
{
namespace mdl
{
class BoneSet;
class NodeSet;

/**
 * @brief The set of animations on a model
 *
 * @ingroup Models
 */
class AnimationSet {
public:
    /**
     * @brief Creates an empty animation set
     */
    AnimationSet() = default;

    /**
     * @brief Populates the animation set from the given scene
     *
     * @param scene The scene to populate from
     * @param nodes The nodes in the model
     * @param bones The bones in the model
     */
    void populate(const aiScene* scene, const NodeSet& nodes, const BoneSet& bones);

    /**
     * @brief Returns the animations in the set
     */
    const std::vector<Animation>& getAnimations() const { return animations; }

private:
    std::vector<Animation> animations;
};

} // namespace mdl
} // namespace bl

#endif
