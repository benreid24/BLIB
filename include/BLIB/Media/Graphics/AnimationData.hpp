#ifndef BLIB_MEDIA_ANIMATIONDATA_HPP
#define BLIB_MEDIA_ANIMATIONDATA_HPP

#include <BLIB/Resources.hpp>

#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

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
     * @brief Loads the AnimationData from the given file. Spritesheets are searched for in the
     *        same directory as the animation file. If not found then
     *        engine::Configuration::get("blib.animation.spritesheet_path") is used
     *
     * @param filename File to laod from
     */
    AnimationData(const std::string& filename);

    /**
     * @brief Loads the animation from the given file, overwriting internal data
     *        Spritesheets are searched for in the same directory as the file. If not found then
     *        engine::Configuration::get("blib.animation.spritesheet_path") is used
     *
     * @param filename The file to load from
     * @param spritesheetDir Optional additional directory to search for spritesheets
     * @return bool True if the animation could be loaded, false otherwise
     */
    bool load(const std::string& filename);

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
    sf::Vector2f getFrameSize(unsigned int frameIndex) const;

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

            void apply(sf::Sprite& sprite, const sf::Vector2f& scale = {1, 1}, float rotation = 0,
                       bool center = true) const;
        };

        float length;
        std::vector<Shard> shards;
    };

    std::string spritesheetSource;
    resource::Resource<sf::Texture>::Ref spritesheet;
    std::vector<Frame> frames;
    float totalLength;
    bool loop;

    void render(sf::RenderTarget& target, sf::RenderStates states, float elapsedTime,
                const sf::Vector2f& position, const sf::Vector2f& scale, float rotation,
                bool centerOnOrigin, bool loopOverride = false, bool loop = false) const;

    friend class Animation;
};

} // namespace gfx
} // namespace bl

#endif
