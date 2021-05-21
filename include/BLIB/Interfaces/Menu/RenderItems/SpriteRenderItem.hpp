#ifndef BLIB_MENU_RENDERITEMS_SPRITERENDERITEM_HPP
#define BLIB_MENU_RENDERITEMS_SPRITERENDERITEM_HPP

#include <BLIB/Interfaces/Menu/RenderItem.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace bl
{
namespace menu
{
/**
 * @brief RenderItem for displaying sprites in a Menu
 *
 * @ingroup Menu
 *
 */
class SpriteRenderItem : public RenderItem {
public:
    typedef std::shared_ptr<SpriteRenderItem> Ptr;

    /**
     * @brief Create the render item from a sprite
     *
     */
    static Ptr create(const sf::Sprite& sprite);

    /**
     * @brief Returns a modifiable reference to the contained sprite
     *
     */
    sf::Sprite& getSprite();

    /**
     * @see RenderItem::getSize
     *
     */
    virtual sf::Vector2f getSize() const override;

    /**
     * @see RenderItem::render
     *
     */
    virtual void render(sf::RenderTarget& target, sf::RenderStates states,
                        const sf::Vector2f& position) const override;

private:
    mutable sf::Sprite sprite;

    SpriteRenderItem(const sf::Sprite& sprite);
};

} // namespace menu
} // namespace bl

#endif
