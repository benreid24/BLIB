#ifndef BLIB_MENU_ITEMS_IMAGEITEM_HPP
#define BLIB_MENU_ITEMS_IMAGEITEM_HPP

#include <BLIB/Interfaces/Menu/Item.hpp>
#include <BLIB/Resources.hpp>
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
class ImageItem : public Item {
public:
    typedef std::shared_ptr<ImageItem> Ptr;

    /**
     * @brief Create the render item from a sprite
     *
     */
    static Ptr create(const resource::Resource<sf::Texture>::Ref& texture);

    /**
     * @brief Returns a modifiable reference to the contained sprite
     *
     */
    sf::Sprite& getSprite();

    /**
     * @brief Changes the texture of this image. Does not trigger a menu refresh
     *
     */
    void setTexture(const resource::Resource<sf::Texture>::Ref& texture);

protected:
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
    resource::Resource<sf::Texture>::Ref texture;
    sf::Sprite sprite;

    ImageItem(const resource::Resource<sf::Texture>::Ref& texture);
};

} // namespace menu
} // namespace bl

#endif
