#ifndef BLIB_MEDIA_ANIMATIONDATA_HPP
#define BLIB_MEDIA_ANIMATIONDATA_HPP

#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

namespace bl
{
class Animation;

/**
 * @brief Data class for animations. Holds the frame information and is resposnsibl for
 * rendering
 * @see Animation
 * @ingroup Media
 *
 */
class AnimationData {
public:
    typedef std::shared_ptr<AnimationData> Ptr;

    static Ptr create();
    static Ptr create(const std::string& filename, const std::string& spritesheetDir = "");
    bool load(const std::string& filename, const std::string& spritesheetDir = "");

    bool isLooping() const;
    float getLength() const;
    unsigned int frameCount() const;
    sf::Vector2f getFrameSize(unsigned int frameIndex) const;
    sf::Vector2f getMaxSize() const;

private:
    struct Frame {
        struct Shard {
            sf::IntRect source;
            sf::Vector2f posOffset;
            sf::Vector2f scale;
            float rotation;
            uint8_t alpha;

            void apply(sf::Sprite& sprite, const sf::Vector2f& scale = {1, 1},
                       float rotation = 0, bool center = true) const;
        };

        float length;
        std::vector<Shard> shards;
    };

    sf::Texture spritesheet;
    std::vector<Frame> frames;
    float totalLength;
    bool loop;

    AnimationData();
    AnimationData(const std::string& filename, const std::string& spritesheetDir = "");

    void render(sf::RenderTarget& target, sf::RenderStates states, float elapsedTime,
                const sf::Vector2f& position, const sf::Vector2f& scale, float rotation,
                bool centerOnOrigin, bool loopOverride = false, bool loop = false) const;

    friend class Animation;
};

} // namespace bl

#endif