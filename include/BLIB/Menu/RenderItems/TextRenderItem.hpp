#ifndef BLIB_MENU_RENDERITEMS_TEXTRENDERITEM_HPP
#define BLIB_MENU_RENDERITEMS_TEXTRENDERITEM_HPP

#include <BLIB/Menu/RenderItem.hpp>
#include <SFML/Graphics/Text.hpp>

namespace bl
{
namespace menu
{
/**
 * @brief RenderItem to display text
 *
 * @ingroup Menu
 *
 */
class TextRenderItem : public RenderItem {
public:
    typedef std::shared_ptr<TextRenderItem> Ptr;

    /**
     * @brief Create a new TextRenderItem from the sf::Text object
     *
     */
    static Ptr create(const sf::Text& text);

    /**
     * @brief Returns a modifiable reference to the Text object
     *
     */
    sf::Text& getTextObject();

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
    mutable sf::Text text;

    TextRenderItem(const sf::Text& text);
};

} // namespace menu
} // namespace bl

#endif
