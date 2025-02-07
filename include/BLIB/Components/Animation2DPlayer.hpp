#ifndef BLIB_COMPONENTS_ANIMATION2DPLAYER_HPP
#define BLIB_COMPONENTS_ANIMATION2DPLAYER_HPP

#include <BLIB/Graphics/Animation2D/AnimationData.hpp>
#include <BLIB/Render/Buffers/AlignedBuffer.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <BLIB/Resources/Ref.hpp>
#include <BLIB/Util/VectorRef.hpp>

namespace bl
{
namespace sys
{
class Animation2DSystem;
}

namespace com
{
/**
 * @brief Owns play state for 2d animations. Intended to be owned by entities with animations, and
 *        can be shared by multiple entities to share play state
 *
 * @ingroup Components
 */
struct Animation2DPlayer {
    static constexpr std::uint32_t InvalidIndex = std::numeric_limits<std::uint32_t>::max();

    /**
     * @brief Deleted
     */
    Animation2DPlayer() = delete;

    /**
     * @brief Creates a player for the given animation
     *
     * @param animation The animation to play
     * @param forSlideshow Indicates where to upload animation data to the Animation2DSystem
     * @param play True to begin playing immediately
     * @param forceLoop True to always loop, false to defer to animation loop setting
     */
    Animation2DPlayer(const resource::Ref<gfx::a2d::AnimationData>& animation, bool forSlideshow,
                      bool play, bool forceLoop = false);

    /**
     * @brief Starts playing the animation, optionally restarting it
     *
     * @param restart True to restart, false to resume
     */
    void play(bool restart = false);

    /**
     * @brief Starts playing the animation from the beginning and loops it, regardless of the
     *        underlying loop setting
     *
     * @param loop Whether to force loop or force single play
     * @param restart True to restart, false to resume
     */
    void playLooping(bool loop = true, bool restart = false);

    /**
     * @brief Returns whether this player loops or not
     */
    bool isLooping() const;

    /**
     * @brief Returns whether the player is playing or not
     */
    bool playing() const { return isPlaying; }

    /**
     * @brief Returns the animation being played
     */
    const resource::Ref<gfx::a2d::AnimationData>& getAnimation() const { return animation; }

    /**
     * @brief Returns whether the animation being played is a slideshow or not
     */
    bool isForSlideshow() const { return forSlideshow; }

    /**
     * @brief Returns the index of this player in the Animation2DSystem system
     */
    std::uint32_t getPlayerIndex() const { return playerIndex; }

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

private:
    const bool forSlideshow;
    resource::Ref<gfx::a2d::AnimationData> animation;
    bool isPlaying;
    bool forceLoop;
    bool loop;
    std::size_t currentState;
    std::size_t currentFrame;
    float frameTime;

    // assigned by Animation2DSystem
    std::uint32_t playerIndex;

    // in descriptor set
    util::VectorRef<std::uint32_t, rc::buf::AlignedBuffer<std::uint32_t>> framePayload;

    // assigned by Animation2DSystem to keep texture alive
    rc::res::TextureRef texture;

    friend class sys::Animation2DSystem;
};

} // namespace com
} // namespace bl

#endif
