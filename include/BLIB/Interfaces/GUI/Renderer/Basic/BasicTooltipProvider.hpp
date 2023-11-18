#ifndef BLIB_GUI_RENDERER_BASIC_BASICTOOLTIPPROVIDER_HPP
#define BLIB_GUI_RENDERER_BASIC_BASICTOOLTIPPROVIDER_HPP

#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Graphics/Text.hpp>
#include <BLIB/Interfaces/GUI/Renderer/TooltipProvider.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
/**
 * @brief Renders tooltips using a basic rectangle background and text with nice defaults
 *
 * @ingroup GUI
 */
class BasicTooltipProvider : public rdr::TooltipProvider {
public:
    /**
     * @brief Does some basic initialization
     */
    BasicTooltipProvider();

    /**
     * @brief Destroys the provider
     */
    virtual ~BasicTooltipProvider() = default;

    /**
     * @brief Updates the provider
     *
     * @param dt Elapsed time in seconds
     */
    virtual void update(float dt) override;

    /**
     * @brief Called when an element needs a tooltip displayed
     *
     * @param element The element to show the tooltip for
     */
    virtual void displayTooltip(Element* element, const glm::vec2& mousePos) override;

    /**
     * @brief Called when the current tooltip, if any, should be dismissed
     */
    virtual void dismissTooltip() override;

    /**
     * @brief Creates the entities required to render tooltips
     *
     * @param engine The game engine instance
     */
    virtual void doCreate(engine::Engine& engine) override;

    /**
     * @brief Called when the provider should be added to a scene
     *
     * @param overlay The scene to add to
     */
    virtual void doSceneAdd(rc::Overlay* overlay) override;

    /**
     * @brief Called when the provider should be removed from the scene
     */
    virtual void doSceneRemove() override;

    /**
     * @brief Called when an element is destroyed or removed
     *
     * @param element The element being destroyed or removed
     */
    virtual void notifyDestroyed(const Element* element) override;

private:
    Element* currentElement;
    gfx::Rectangle box;
    gfx::Text text;
};

} // namespace defcoms
} // namespace gui
} // namespace bl

#endif
