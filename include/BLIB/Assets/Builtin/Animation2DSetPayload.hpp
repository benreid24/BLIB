#ifndef BLIB_ASSETS_BUILTIN_ANIMATION2DSETPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_ANIMATION2DSETPAYLOAD_HPP

#include <BLIB/Assets/Builtin/Animation2DPayload.hpp>
#include <BLIB/Assets/DependencyList.hpp>
#include <optional>

namespace bl
{
namespace asi
{
class Animation2DSetDriver;

/**
 * @brief Set of 2d animations. Used to allow multiple animations to be applied to a single entity
 *        while keeping data locality high
 *
 * @ingroup Assets
 */
class Animation2DSetPayload : public as::Payload {
public:
    /**
     * @brief Custom data required to create an animation set
     */
    struct CreateData : public as::CreateContext::CreateData {
        as::TypedRef<Animation2DPayload> baseAnimation;
        std::vector<as::TypedRef<Animation2DPayload>> additionalAnimations;
        bool enforceSlideshow = false;

        struct DebugCreate {
            as::TypedRef<ImagePayload> spritesheet;
            std::vector<Animation2DPayload::Frame>& frames;
            bool loop;
            bool centerShards;

            DebugCreate(as::TypedRef<ImagePayload> spritesheet,
                        std::vector<Animation2DPayload::Frame>& frames, bool loop,
                        bool centerShards)
            : spritesheet(spritesheet)
            , frames(frames)
            , loop(loop)
            , centerShards(centerShards) {}
        };

        std::optional<DebugCreate> debugCreateData;
    };

    /**
     * @brief Creates an empty animation set payload
     *
     * @param ctx The construction context
     */
    Animation2DSetPayload(const as::Payload::ConstructContext& ctx);

    /**
     * @brief Destroys the payload
     */
    virtual ~Animation2DSetPayload() = default;

    /**
     * @brief Returns the spritesheet for this animation set
     */
    as::TypedRef<ImagePayload> getSpritesheet() const;

    /**
     * @brief Returns if the animation is set to loop on end. Can be overridden in Animation
     */
    bool isLooping() const;

    /**
     * @brief Returns the total length of the animation in seconds
     */
    float getLength() const;

    /**
     * @brief Returns whether or not the animation is a slideshow
     */
    bool isSlideshow() const;

    /**
     * @brief Returns whether or not shards should be centered
     */
    bool shardsAreCentered() const;

    /**
     * @brief Returns the number of frames in the animation
     */
    std::size_t frameCount() const;

    /**
     * @brief Fetch a specific frame within the animation
     *
     * @param frameIndex The frame index to fetch
     * @return A reference to the given frame
     */
    const Animation2DPayload::Frame& getFrame(std::size_t frameIndex) const;

    /**
     * @brief Returns the width and height of the given frame
     *
     * @param frameIndex Index of the frame to check
     * @return sf::Vector2f Size of the frame
     */
    const sf::Vector2f& getFrameSize(std::size_t frameIndex) const;

    /**
     * @brief Returns the size of the largest frame
     */
    sf::Vector2f getMaxSize() const;

    /**
     * @brief Returns the index of the next frame from the current frame
     *
     * @param currentFrame The current frame
     * @return The frame that comes after the current frame
     */
    std::size_t getNextFrame(std::size_t currentFrame) const;

    /**
     * @brief Returns the length of the given frame in seconds
     *
     * @param frameIndex The frame to get the length of
     * @return The length of the frame in seconds
     */
    float getFrameLength(std::size_t frameIndex) const;

    /**
     * @brief Combines the stateAnim into this one and updates the next frame lookups. May fail if
     *        the animation is not compatible with this one. Compatible animations have the same
     *        number of shards per frame. Same spritesheet must also be used. State 0 is always this
     *        base animation
     *
     * @param expectedStateIndex State index to validate. Should be the current number of states
     * @param stateAnim The animation to combine into this one
     * @param enforceSlideshow True to also validate that slideshow status is the same or better
     * @return True if the animation could be combined, false on error
     */
    bool addState(std::size_t expectedStateIndex, as::TypedRef<Animation2DPayload> stateAnim,
                  bool enforceSlideshow = true);

    /**
     * @brief Returns the starting frame for the given state
     *
     * @param stateIndex The 0-based index of the state to get the frame index for
     * @return The frame index where the requested state animation begins
     */
    std::size_t getFrameForState(std::size_t stateIndex) const;

    /**
     * @brief Returns the state index for the given frame. Even animations with no states added have
     *        an implicit 0 state
     *
     * @param frameIndex The frame to get the state of
     * @return The state index for the given frame
     */
    std::size_t getStateFromFrame(std::size_t frameIndex) const;

private:
    as::DependencyList<Animation2DPayload> animations;

    // computed
    std::size_t totalFrames;
    std::vector<std::size_t> stateOffsets;

    void computeDerivedState();

    friend class Animation2DSetDriver;
};

} // namespace asi
} // namespace bl

#endif
