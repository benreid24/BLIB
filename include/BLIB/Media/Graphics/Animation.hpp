#ifndef BLIB_MEDIA_ANIMATION_HPP
#define BLIB_MEDIA_ANIMATION_HPP

#include <BLIB/Graphics/Animation2D/AnimationData.hpp>

namespace bl
{
namespace rc
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
     * @brief Starts playing the animation. Restarts if not currently playing
     *
     * @param restart True to restart if already playing, false to keep place
     *
     */
    void play(bool restart = true);

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

    /**
     * @brief Moves the animation forward by dt seconds if playing
     *
     * @param dt Time elapsed since last call to update
     */
    void update(float dt);

    /**
     * @brief Additional rendering function that allows residual time to be passed in. Residual time
     *        is elapsed real time between calls to update()
     *
     * @param target The target to render to
     * @param residual Time elapsed that is not accounted for in calls to update()
     * @param states Additional render states for SFML
     */
    void render(sf::RenderTarget& target, float residual, sf::RenderStates states = {}) const;

private:
    AnimationData* data;
    bool isPlaying;
    unsigned int frame;
    float frameTime;

    sf::Vector2f position;
    sf::Vector2f scale;
    float rotation;
    bool loopOverride;
    bool loop;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

} // namespace rc
} // namespace bl

#endif
