#ifndef BLIB_GUI_RENDERER_RENDERER_HPP
#define BLIB_GUI_RENDERER_RENDERER_HPP

#include <BLIB/Interfaces/GUI/Renderer/FactoryTable.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <unordered_map>

namespace bl
{
namespace engine
{
class Engine;
}
namespace gui
{
class GUI;
class Element;

namespace rdr
{
/**
 * @brief Top level GUI renderer. Manages components for elements. Behavior can be controlled by
 *        providing a different FactoryTable to the GUI
 *
 * @ingroup GUI
 */
class Renderer {
public:
    /**
     * @brief Creates the GUI renderer
     *
     * @param engine The game engine instance
     * @param gui The GUI instance
     * @param factory The component factory to use
     */
    Renderer(engine::Engine& engine, GUI& gui, FactoryTable& factory);

    /**
     * @brief Creates a component for the given GUI element. Adds it to the current scene if any
     *
     * @tparam TElem The type of element to create the component for
     * @param element The element to create the component for
     * @param parent The parent component of the component to create
     * @param windowOrGui The element's parent window, or the GUI's element if not in a window
     * @return A pointer to the new component
     */
    template<typename TElem>
    Component* createComponent(TElem& element, Component* parent, Component* windowOrGui) {
        const auto it = components.try_emplace(&element, factory.createComponent<TElem>()).first;
        it->second->create(engine, *this, element, parent, windowOrGui);
        if (overlay) { it->second->doSceneAdd(overlay); }
        return it->second.get();
    }

    /**
     * @brief Destroys the component for the given element
     *
     * @param owner The owner of the element to destroy
     */
    void destroyComponent(const Element& owner);

    /**
     * @brief Adds all current and future components to the given overlay
     *
     * @param overlay The overlay to add to
     */
    void addToOverlay(rc::Overlay* overlay);

    /**
     * @brief Removes all components from the current overlay, if any
     */
    void removeFromOverlay();

    /**
     * @brief Displays the tooltip for the given element
     *
     * @param src The element to display the tooltip for
     */
    void displayTooltip(Element* src);

    /**
     * @brief Dismisses the tooltip for the given element
     *
     * @param src The element to dismiss the tooltip for
     */
    void dismissTooltip(Element* src);

    /**
     * @brief Flashes the given element
     *
     * @param src The element to flash
     */
    void flash(Element* src);

    /**
     * @brief Called when an element changes UI state
     *
     * @param src The element changing state
     * @param state The new UI state of the element
     */
    void handleComponentState(Element* src, Component::UIState state);

private:
    engine::Engine& engine;
    GUI& gui;
    FactoryTable& factory;
    rc::Overlay* overlay;
    std::unordered_map<const Element*, Component::Ptr> components;

    FlashProvider::Ptr flashProvider;
    HighlightProvider::Ptr highlightProvider;
    TooltipProvider::Ptr tooltipProvider;
    Element* currentTooltip;
};

} // namespace rdr
} // namespace gui
} // namespace bl

#endif
