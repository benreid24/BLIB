#ifndef BLIB_GUI_RENDERER_BASIC_OVERLAYHIGHLIGHTPROVIDER_HPP
#define BLIB_GUI_RENDERER_BASIC_OVERLAYHIGHLIGHTPROVIDER_HPP

#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Interfaces/GUI/Renderer/HighlightProvider.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
/**
 * @brief Provides mouse highlights by rendering a partially transparent rectangle over the target
 *
 * @ingroup GUI
 */
class OverlayHighlightProvider : public rdr::HighlightProvider {
public:
    /**
     * @brief Creates the provider
     */
    OverlayHighlightProvider();

    /**
     * @brief Destroys the provider
     */
    virtual ~OverlayHighlightProvider() = default;

    /**
     * @brief Does nothing
     *
     * @param dt Elapsed time in seconds
     */
    virtual void update(float dt) override;

    /**
     * @brief Called when an element changes state and needs a new highlight
     *
     * @param element The element changing state
     * @param state The new state of the element
     */
    virtual void notifyUIState(Element* element, rdr::Component::UIState state) override;

    /**
     * @brief Creates the overlay rectangle
     *
     * @param world The world to create entities in
     */
    virtual void doCreate(engine::World& world) override;

    /**
     * @brief Adds the rectangle to the scene
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
    engine::Engine* enginePtr;
    gfx::Rectangle cover;
    Element* currentElement;
    rdr::Component::UIState currentState;
};

} // namespace defcoms
} // namespace gui
} // namespace bl

#endif
