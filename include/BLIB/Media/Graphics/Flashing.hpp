#ifndef BLIB_MEDIA_GRAPHICS_FLASHING_HPP
#define BLIB_MEDIA_GRAPHICS_FLASHING_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Wrapper class for drawable elements that should flash with a fixed period
 *
 * @ingroup Graphics
 *
 */
class Flashing {
public:
    /**
     * @brief Construct a new Flashing component
     *
     * @param component The renderable element to flash
     * @param onPeriod How long to be visible, in seconds
     * @param offPeriod How long to be hidden, in seconds
     */
    Flashing(sf::Drawable& component, float onPeriod, float offPeriod);

    /**
     * @brief Updates the state of the flasher
     *
     * @param dt Time elapsed, in seconds
     */
    void update(float dt);

    /**
     * @brief Renders the flashing element if it is visible
     *
     * @param target The target to render to
     * @param states Render states to use
     * @param lag Time elapsed, in seconds, not accounted for in update
     */
    void render(sf::RenderTarget& target, sf::RenderStates states, float lag) const;

private:
    const sf::Drawable& component;
    const float onPeriod;
    const float offPeriod;
    float time;
    bool visible;
};

} // namespace gfx
} // namespace bl

#endif
