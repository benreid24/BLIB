#ifndef BLIB_MENU_SELECTOR_HPP
#define BLIB_MENU_SELECTOR_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <memory>

namespace bl
{
namespace menu
{
/**
 * @brief Base class for selector items that indicate which Menu item is active
 *
 * @ingroup Menu
 *
 */
class Selector {
public:
    typedef std::shared_ptr<Selector> Ptr;

    virtual ~Selector() = default;

    /**
     * @brief Render the selection indicator to the item in the given screen region
     *
     * @param target Target to render to
     * @param states Render states to use
     * @param itemArea Area the item is in which needs to be indicated
     */
    virtual void render(sf::RenderTarget& target, sf::RenderStates states,
                        sf::FloatRect itemArea) = 0;
};

} // namespace menu
} // namespace bl

#endif
