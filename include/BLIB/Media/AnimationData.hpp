#ifndef BLIB_MEDIA_ANIMATIONDATA_HPP
#define BLIB_MEDIA_ANIMATIONDATA_HPP

#include <string>
#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>

namespace bl
{

/**
 * @brief Data class for animations. Holds the frame information and is resposnsibl for rendering
 * @see Animation
 * @ingroup Media
 * 
 */
class AnimationData {
public:
    AnimationData();
    AnimationData(const std::string& filename, const std::string& spritesheetDir = "");
    bool load(const std::string& filename, const std::string& spritesheetDir = "");

    bool isLooping() const;
    float getLength() const;
    unsigned int frameCount() const;
    sf::Vector2f getFrameSize(unsigned int frameIndex) const;
    sf::Vector2f getMaxSize() const;

    void render(sf::RenderTarget& target, float elapsedTime,
        const sf::Vector2f& position, const sf::Vector2f& scale,
        float rotation, bool centerOnOrigin,
        bool loopOverride = false, bool loop = false) const;

private:
    struct Frame;

    sf::Texture spritesheet;
    std::vector<std::shared_ptr<Frame>> frames;
    float totalLength;
    bool loop;
};

}

#endif