#ifndef BLIB_COMPONENTS_ANIMATION2DPLAYER_HPP
#define BLIB_COMPONENTS_ANIMATION2DPLAYER_HPP

#include <BLIB/Graphics/Animation2D/AnimationData.hpp>
#include <BLIB/Render/Vulkan/AlignedBuffer.hpp>
#include <BLIB/Resources/Ref.hpp>
#include <BLIB/Util/VectorRef.hpp>

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
     * @brief Deleted
     */
    Animation2DPlayer() = delete;

    /**
     * @brief Creates a player for the given animation
     *
     * @param animation The animation to play
     * @param play True to begin playing immediately
     * @param forceLoop True to always loop, false to defer to animation loop setting
     */
    Animation2DPlayer(const resource::Ref<gfx::a2d::AnimationData>& animation, bool play,
                      bool forceLoop = false);

    /**
     * @brief Starts playing the animation, optionally restarting it
     *
     * @param restart True to restart, false to resume
     */
    void play(bool restart = false);

    /**
     * @brief Starts playing the animation from the beginning and loops it, regardless of the
     *        underlying loop setting
     */
    void playLooping();

    /**
     * @brief Pauses the animation from playing
     */
    void pause();

    /**
     * @brief Stops and resets the animation
     */
    void stop();

    /**
     * @brief Sets the state to play within the underlying animation
     *
     * @param state The state index to play
     * @param play True to begin playing, false to stay stopped
     */
    void setState(std::size_t state, bool play = true);

    /**
     * @brief Advances the animation forward by the given amount of time
     *
     * @param dt Elapsed time in seconds
     */
    void update(float dt);

    resource::Ref<gfx::a2d::AnimationData> animation;
    bool isPlaying;
    bool forceLoop;
    std::size_t currentState;
    std::size_t currentFrame;
    float frameTime;

    // TODO - stable vector ref
    std::uint32_t playerIndex; // assigned by Animation2DSystem
    util::VectorRef<std::uint32_t, rc::vk::AlignedBuffer<std::uint32_t>>
        framePayload; // in descriptor set
};

} // namespace com
} // namespace bl

#endif
