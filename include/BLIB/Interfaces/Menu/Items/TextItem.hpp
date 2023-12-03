#ifndef BLIB_MENU_RENDERITEMS_TEXTITEM_HPP
#define BLIB_MENU_RENDERITEMS_TEXTITEM_HPP

#include <BLIB/Graphics/Text.hpp>
#include <BLIB/Interfaces/Menu/Item.hpp>

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
class TextItem : public Item {
public:
    typedef std::shared_ptr<TextItem> Ptr;

    /**
     * @brief Create a new TextItem
     *
     * @param text The string to display
     * @param font The font to use
     * @param color The color of the text
     * @param fontSize The font size
     * @param style The style of the text
     * @return Ptr The created text menu item
     */
    static Ptr create(const std::string& text, const sf::VulkanFont& font,
                      const sf::Color& color = sf::Color::Black, unsigned int fontSize = 30,
                      std::uint32_t style = sf::Text::Regular);

    /**
     * @brief Destroy the Text Item object
     */
    virtual ~TextItem() = default;

    /**
     * @brief Returns a modifiable reference to the Text object
     */
    gfx::Text& getTextObject();

    /**
     * @see Item::getSize
     */
    virtual glm::vec2 getSize() const override;

protected:
    /**
     * @brief Create a new TextItem
     *
     * @param text The string to display
     * @param font The font to use
     * @param color The color of the text
     * @param fontSize The font size
     * @param style The style of the text
     */
    TextItem(const std::string& text, const sf::VulkanFont& font, const sf::Color& color,
             unsigned int fontSize, std::uint32_t style);

    /**
     * @brief Called at least once when the item is added to a menu. Should create required graphics
     *        primitives and return the transform to use
     *
     * @param engine The game engine instance
     * @param parent The parent entity that should be used
     * @return The transform component to use
     */
    virtual com::Transform2D& doCreate(engine::Engine& engine, ecs::Entity parent) override;

    /**
     * @brief Called when the item should be added to the overlay
     *
     * @param overlay The overlay to add to
     */
    virtual void doSceneAdd(rc::Overlay* overlay) override;

    /**
     * @brief Called when the item should be removed from the overlay
     */
    virtual void doSceneRemove() override;

    /**
     * @brief Returns the entity (or top level entity) of the item
     */
    virtual ecs::Entity getEntity() const override;

private:
    const std::string string;
    const sf::VulkanFont& font;
    const sf::Color color;
    const unsigned int fontSize;
    const std::uint32_t style;
    gfx::Text text;
};

} // namespace menu
} // namespace bl

#endif
