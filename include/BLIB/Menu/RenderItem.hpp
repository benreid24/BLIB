#ifndef BLIB_MENU_RENDERITEM_HPP
#define BLIB_MENU_RENDERITEM_HPP

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

namespace bl
{
namespace menu
{
/**
 * @brief Abstract base class for any Item that can be rendered by a Menu
 * 
 * @ingroup Menu
 * @see TextRenderItem
 * @see SpriteRenderItem
 * 
 */
class RenderItem {
public:
    /**
     * @brief Return the untransformed size of the object
     * 
     * @return sf::Vector2f Size the object will take
     */
    virtual sf::Vector2f getSize() const                    = 0;

    /**
     * @brief Render the item to the given target
     * 
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param position Position to render at
     */
    virtual void render(sf::RenderTarget& target, sf::RenderStates states,
                        const sf::Vector2f& position) const = 0;
};

} // namespace menu
} // namespace bl

#endif
