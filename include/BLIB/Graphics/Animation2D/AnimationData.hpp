#ifndef BLIB_GRAPHICS_ANIMATION2D_ANIMATIONDATA_HPP
#define BLIB_GRAPHICS_ANIMATION2D_ANIMATIONDATA_HPP

#include <BLIB/Serialization.hpp>
#include <memory>
#include <string>
#include <vector>

namespace bl
{
namespace gfx
{
namespace a2d
{
/**
 * @brief Data class for 2d animations. Provides the backing for slideshows and skeletal animations.
 *        Multiple AnimationData objects can be combined to create a stateful animation
 *
 * @see Animation
 * @ingroup Media
 */
class AnimationData {
public:
    /**
     * @brief Struct containing a single frame within an animation
     */
    struct Frame {
        /**
         * @brief A skeleton bone within an animation frame
         */
        struct Shard {
            sf::IntRect source;
            sf::Vector2f offset;
            sf::Vector2f scale;
            float rotation;
            std::uint8_t alpha;
        };

        std::size_t nextFrame;
        std::vector<Shard> shards;
        sf::Vector2f size;
        float length;
    };

    /**
     * @brief Returns whether or not the animation is a slideshow. A slideshow is an animation with
     *        one shard in each frame that only changes texture coordinates
     *
     * @param data The AnimationData to check
     * @return True if the animation is a slideshow, false if it is skeletal
     */
    static bool isValidSlideshow(const AnimationData& data);

    /**
     * @brief Create empty AnimationData to be loaded later
     *
     */
    AnimationData();

    /**
     * @brief Loads the animation from the given file, overwriting internal data
     *        Spritesheets are searched for in the same directory as the file. If not found then
     *        engine::Configuration::get("blib.animation.spritesheet_path") is used
     *
     * @param filename The file to load from
     * @param spritesheetDir Optional additional directory to search for spritesheets
     * @return bool True if the animation could be loaded, false otherwise
     */
    bool loadFromFile(const std::string& filename);

    /**
     * @brief Same as loadFromFile, but rewrites the spritesheet source to be the full path to the
     *        spritesheet so that it may be directly added to the bundle
     *
     * @param filename Path to the animation to load
     * @return True if the animation could be loaded, false otherwise
     */
    bool loadFromFileForBundling(const std::string& filename);

    /**
     * @brief Saves the animation data to the bundle
     *
     * @param output The bundle to save to
     * @return True if the data was written, false on error
     */
    bool saveToBundle(std::ostream& output) const;

    /**
     * @brief Loads the AnimationData from the given file. Spritesheets are searched for in the
     *        same directory as the animation file. If not found then
     *        engine::Configuration::get("blib.animation.spritesheet_path") is used
     *
     * @param buffer The memory buffer to load from
     * @param len Size of the buffer to load from
     * @param originalPath The original file path of the animation. Used to help locate spritesheet
     * @return True if the animation could be loaded, false on error
     */
    bool loadFromMemory(const char* buffer, std::size_t len, const std::string& originalPath);

    /**
     * @brief Returns the filename of the spritesheet
     *
     */
    const std::string& spritesheetFile() const;

    /**
     * @brief Returns if the animation is set to loop on end. Can be overridden in Animation
     *
     */
    bool isLooping() const;

    /**
     * @brief Returns the total length of the animation in seconds
     *
     */
    float getLength() const;

    /**
     * @brief Returns the number of frames in the animation
     *
     */
    std::size_t frameCount() const;

    /**
     * @brief Fetch a specific frame within the animation
     *
     * @param frameIndex The frame index to fetch
     * @return A reference to the given frame
     */
    const Frame& getFrame(std::size_t frameIndex) const;

    /**
     * @brief Returns the width and height of the given frame
     *
     * @param frameIndex Index of the frame to check
     * @return sf::Vector2f Size of the frame
     */
    const sf::Vector2f& getFrameSize(std::size_t frameIndex) const;

    /**
     * @brief Returns the size of the largest frame
     *
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
    float getFrameLength(std::size_t frameIndex);

    /**
     * @brief Prepares this animation to be a combination of animations for various possible entity
     *        states
     *
     * @param stateCount The number of different states that will be added
     */
    void prepareForStates(std::size_t stateCount);

    /**
     * @brief Combines the stateAnim into this one and updates the next frame lookups. May fail if
     *        the animation is not compatible with this one. Compatible animations have the same
     *        number of shards per frame. Same spritesheet must also be used. State 0 is always this
     *        base animation
     *
     * @param stateIndex The 0-based index of the state that the given animation is for
     * @param stateAnim The animation to combine into this one
     * @param enforceSlideshow True to also validate that slideshow status is the same or better
     * @return True if the animation could be combined, false on error
     */
    bool addState(std::size_t stateIndex, const AnimationData& stateAnim,
                  bool enforceSlideshow = true);

    /**
     * @brief Returns the starting frame for the given state
     *
     * @param stateIndex The 0-based index of the state to get the frame index for
     * @return The frame index where the requested state animation begins
     */
    std::size_t getFrameForState(std::size_t stateIndex);

    /**
     * @brief Returns the state index for the given frame. Even animations with no states added have
     *        an implicit 0 state
     *
     * @param frameIndex The frame to get the state of
     * @return The state index for the given frame
     */
    std::size_t getStateFromFrame(std::size_t frameIndex) const;

private:
    std::string spritesheetSource;
    std::vector<Frame> frames;
    std::vector<std::size_t> stateOffsets;
    float totalLength;
    bool loop;
    bool centerShards;

    static void computeFrameSize(Frame& frame);
    bool doLoad(serial::binary::InputStream& input, const std::string& path, bool forBundle);

    friend class Animation;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline std::size_t AnimationData::frameCount() const { return frames.size(); }

inline const AnimationData::Frame& AnimationData::getFrame(std::size_t i) const {
    return frames[i];
}

inline std::size_t AnimationData::getNextFrame(std::size_t currentFrame) const {
    return frames[currentFrame].nextFrame;
}

inline float AnimationData::getFrameLength(std::size_t i) { return frames[i].length; }

inline std::size_t AnimationData::getFrameForState(std::size_t i) { return stateOffsets[i]; }

inline const sf::Vector2f& AnimationData::getFrameSize(std::size_t i) const {
    return frames[i].size;
}

} // namespace a2d
} // namespace gfx
} // namespace bl

#endif
