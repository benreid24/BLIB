#ifndef BLIB_MENU_SELECTORS_ARROWSELECTOR_HPP
#define BLIB_MENU_SELECTORS_ARROWSELECTOR_HPP

#include <BLIB/Graphics/Triangle.hpp>
#include <BLIB/Interfaces/Menu/Selector.hpp>

namespace bl
{
namespace menu
{
/**
 * @brief Basic selection indicator, draws a triangle to the left of the selected item
 *
 * @ingroup Menu
 */
class ArrowSelector : public Selector {
public:
    typedef std::shared_ptr<ArrowSelector> Ptr;

    /**
     * @brief Create a new ArrowSelector with the given width
     *
     * @param width The width of the arrow in pixels
     * @param fill Color to fill the arrow with
     */
    static Ptr create(float width, const sf::Color& fill = sf::Color::White);

    /**
     * @brief Destroys the arrow selector
     */
    virtual ~ArrowSelector() = default;

    /**
     * @brief Exposes the underlying triangle
     */
    gfx::Triangle& getArrow();

    /**
     * @brief Called when the selector is added to a menu
     *
     * @param world The world to create entities in
     * @param parent The parent entity id
     */
    virtual void doCreate(engine::World& world, ecs::Entity parent) override;

    /**
     * @brief Called when the selector should select an item
     *
     * @param item The ECS id of the selected item
     * @param itemArea The overlay space area of the selected item
     */
    virtual void notifySelection(ecs::Entity item, sf::FloatRect itemArea) override;

    /**
     * @brief Called when the selector should be added to the scene
     *
     * @param scene The overlay to add to
     */
    virtual void doSceneAdd(rc::Scene* scene) override;

    /**
     * @brief Called when the selector should be removed from the scene
     */
    virtual void doSceneRemove() override;

    /**
     * @brief Manually draw the selector
     *
     * @param ctx The render context
     */
    virtual void draw(rc::scene::CodeScene::RenderContext& ctx) override;

private:
    const float width;
    const sf::Color fillColor;
    gfx::Triangle triangle;

    ArrowSelector(float width, const sf::Color& f);
};

} // namespace menu
} // namespace bl

#endif
