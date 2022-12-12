#ifndef BLIB_MEDIA_ANIMATIONDATA_HPP
#define BLIB_MEDIA_ANIMATIONDATA_HPP

#include <BLIB/Media/Graphics/VertexBuffer.hpp>
#include <BLIB/Resources/Resource.hpp>
#include <BLIB/Serialization.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

namespace bl
{
namespace gfx
{
class Animation;

/**
 * @brief Data class for animations. Holds the frame information and is responsible for
 *        rendering
 * @see Animation
 * @ingroup Media
 *
 */
class AnimationData {
public:
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
     * @brief Returns if the animation is set to loop on end. Can be overriden in Animation
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
    unsigned int frameCount() const;

    /**
     * @brief Returns the width and height of the given frame
     *
     * @param frameIndex Index of the frame to check
     * @return sf::Vector2f Size of the frame
     */
    const sf::Vector2f& getFrameSize(unsigned int frameIndex) const;

    /**
     * @brief Returns the size of the largest frame
     *
     */
    sf::Vector2f getMaxSize() const;

private:
    struct Frame {
        struct Shard {
            sf::IntRect source;
            sf::Vector2f posOffset;
            sf::Vector2f scale;
            float rotation;
            uint8_t alpha;

            void apply(VertexBuffer& buffer, unsigned int offset, bool centerShards) const;
        };

        std::vector<Shard> shards;
    };

    std::string spritesheetSource;
    resource::Resource<sf::Texture>::Ref spritesheet;
    std::vector<Frame> frameData;
    std::vector<VertexBuffer> frames;
    std::vector<sf::Vector2f> sizes;
    std::vector<float> lengths;
    float totalLength;
    bool loop;
    bool centerShards;

    void render(sf::RenderTarget& target, sf::RenderStates states, const sf::Vector2f& position,
                const sf::Vector2f& scale, float rotation, unsigned int frame) const;
    sf::Vector2f computeFrameSize(unsigned int i) const;
    bool doLoad(serial::binary::InputStream& input, const std::string& path, bool forBundle);

    friend class Animation;
};

} // namespace gfx
} // namespace bl

#endif
