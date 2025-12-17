#ifndef BLIB_MODELS_ANIMATION_HPP
#define BLIB_MODELS_ANIMATION_HPP

#include <BLIB/Models/BoneAnimation.hpp>
#include <assimp/anim.h>
#include <string>
#include <vector>

namespace bl
{
namespace mdl
{
/**
 * @brief Representa an animation of a model
 *
 * @ingroup Models
 */
class Animation {
public:
    /**
     * @brief Creates an empty animation
     */
    Animation();

    /**
     * @brief Populates the animation from Assimp data
     *
     * @param src The source data
     */
    void populate(const aiAnimation* src);

    /**
     * @brief Returns the name of the animation
     */
    const std::string& getName() const { return name; }

    /**
     * @brief Returns the duration of the animation in ticks
     */
    double getDurationInTicks() const { return durationInTicks; }

    /**
     * @brief Returns the ticks per second of the animation
     */
    double getTicksPerSecond() const { return ticksPerSecond; }

    /**
     * @brief Returns the bone animations in this animation
     */
    const std::vector<BoneAnimation>& getBoneAnimations() const { return boneAnimations; }

private:
    std::string name;
    double durationInTicks;
    double ticksPerSecond;
    std::vector<BoneAnimation> boneAnimations;
};

} // namespace mdl
} // namespace bl

#endif
