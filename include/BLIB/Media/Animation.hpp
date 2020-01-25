#ifndef BLIB_MEDIA_ANIMATION_HPP
#define BLIB_MEDIA_ANIMATION_HPP

#include <BLIB/Media/AnimationData.hpp>

namespace bl
{
/**
 * @brief Represents a renderable animation.
 * AnimationData is to Animation like sf::Texture is to sf::Sprite
 * @ingroup Media
 *
 */
class Animation : public sf::Drawable {
public:
    Animation();
    Animation(const AnimationData& data, bool centerOrigin = true);

    void setData(const AnimationData& data, bool centerOrigin = true);
    const AnimationData* getData() const;

    void setPosition(const sf::Vector2f& position);
    void setScale(const sf::Vector2f& scale);
    void setRotation(float rotation);
    void setIsLoop(bool loop);
    void resetIsLoop();

    void play();
    bool playing() const;
    bool finished() const;
    void stop();

private:
    const AnimationData* data;
    bool isPlaying;
    float startTime;

    bool centerOrigin;
    sf::Vector2f position;
    sf::Vector2f scale;
    float rotation;
    bool loopOverride;
    bool loop;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

} // namespace bl

#endif