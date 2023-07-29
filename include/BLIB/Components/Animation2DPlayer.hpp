#ifndef BLIB_COMPONENTS_ANIMATION2DPLAYER_HPP
#define BLIB_COMPONENTS_ANIMATION2DPLAYER_HPP

#include <BLIB/Graphics/Animation2D/AnimationData.hpp>
#include <BLIB/Resources/Ref.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Owns play state for 2d animations. Intended to be owned by entities with animations, and
 *        can be shared by multiple entities to share play state
 *
 * @ingroup Components
 */
struct Animation2DPlayer {
    /**
     * @brief Creates an empty player
     */
    Animation2DPlayer()
    : currentFrame(0)
    , frameTime(0.f) {}

    resource::Ref<gfx::a2d::AnimationData> animation;
    std::size_t currentFrame;
    float frameTime;
};

} // namespace com
} // namespace bl

#endif
