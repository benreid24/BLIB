#ifndef BLIB_MEDIA_ANIMATION_HPP
#define BLIB_MEDIA_ANIMATION_HPP

#include <BLIB/Media/Graphics/AnimationData.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Represents a renderable animation.
 *        AnimationData is to Animation like sf::Texture is to sf::Sprite.
 *        Animation contains state such as the position and current frame
 * @ingroup Media
 *
 */
class Animation : public sf::Drawable {
public:
    /**
     * @brief Creates an empty animation
     *
     */
    Animation();

    /**
     * @brief Creates an animation on top of the underlying AnimationData
     *
     * @param data The AnimationData to use for rendering
     */
    Animation(AnimationData& data);

    /**
     * @brief Sets the AnimationData to use for rendering
     *
     */
    void setData(AnimationData& data);

    /**
     * @brief Returns the underlying AnimationData
     *
     */
    AnimationData& getData() const;

    /**
     * @brief Set the position to render at
     *
     * @param position
     */
    void setPosition(const sf::Vector2f& position);

    /**
     * @brief Determines if the origin for rendering is the top left or the center of the
     *        current frame
     *
     * @param center True to center, false to use top left
     */
    void setIsCentered(bool center);

    /**
     * @brief Apply a scaling factor when rendering
     *
     */
    void setScale(const sf::Vector2f& scale);

    /**
     * @brief Apply an extra rotation when rendering
     *
     * @param rotation Rotation in degrees
     */
    void setRotation(float rotation);

    /**
     * @brief Override the default loop on finish of the underlying AnimationData
     *
     * @param loop True to loop on finish, false to stop
     */
    void setIsLoop(bool loop);

    /**
     * @brief Clears loop override and uses the AnimationData's setting
     *
     */
    void resetIsLoop();

    /**
     * @brief Starts playing the animation. Resets to first frame if already playing
     *
     */
    void play();

    /**
     * @brief Returns if the animation is currently playing
     *
     */
    bool playing() const;

    /**
     * @brief Returns if the animation was played and then finished
     *        Never true for looping animations
     *
     */
    bool finished() const;

    /**
     * @brief Stops the animation if it is playing
     *        Stopped animations render the first frame if they are drawn
     *
     */
    void stop();

private:
    AnimationData* data;
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

} // namespace gfx
} // namespace bl

#endif
