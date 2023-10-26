#ifndef BLIB_GUI_GUI_HPP
#define BLIB_GUI_GUI_HPP

#include <BLIB/Events.hpp>
#include <BLIB/Interfaces/GUI/Elements/Box.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>
#include <BLIB/Render/Observer.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

/// A window based graphical user interface
namespace gui
{
/**
 * @brief Top level class for creating a graphical user interface
 *
 * A GUI object is what client code must use for manipulating and rendering an interface. A GUI
 * object may only be used in a single OS window. A GUI may be positioned and have a size set.
 * Elements in the GUI will be positioned and constrained by the GUI's position and size. GUI
 * windows are not constrained
 *
 * @ingroup GUI
 */
class GUI
: public bl::event::Listener<sf::Event>
, public gui::Box {
public:
    typedef std::unique_ptr<GUI> Ptr;

    /**
     * @brief Create a new GUI that is in the given region
     *
     * @param engine The game engine instance
     * @param observer The observer the GUI will belong to
     * @param packer The Packer to use
     * @param region The position and size of the renderable area. Defaults to the full overlay
     *               space unless a value is manually set
     * @param factory The component factory to use in the renderer. Must remain valid
     */
    static Ptr create(engine::Engine& engine, rc::Observer& observer,
                      const gui::Packer::Ptr& packer, const sf::FloatRect& region = {},
                      rdr::FactoryTable* factory = nullptr);

    /**
     * @brief Set the Region objectSets the region to pack elements into
     *
     * @param area The new region to fill
     */
    void setRegion(const sf::FloatRect& area);

    /**
     * @brief Handles and propagates the window event
     *
     * @param Packer The Packer to use
     * @param event Raw window event
     */
    virtual void observe(const sf::Event& event) override;

    /**
     * @brief Updates all child elements and runs any queued actions
     *
     * @param dt Time elapsed in seconds
     */
    virtual void update(float dt) override; // TODO - put into system?

    /**
     * @brief Queues an action to be ran after update()
     *
     * @see Element::queueUpdateAction
     * @param action The action to run
     */
    void queueAction(const QueuedAction& action);

    /**
     * @brief Adds the GUI and all contained elements to the given overlay. Passing nullptr will
     *        default to getOrCreateOverlay() on the observer
     *
     * @param overlay The overlay to add to, or nullptr
     */
    void addToOverlay(rc::Overlay* overlay = nullptr);

    /**
     * @brief Removes the GUI and all elements from the current overlay
     */
    void removeFromOverlay();

private:
    rc::Observer& observer;
    std::vector<Element::QueuedAction> queuedActions;
    rdr::Renderer renderer;
    glm::vec2 mousePos;

    GUI(engine::Engine& engine, rc::Observer& observer, const gui::Packer::Ptr& packer,
        const sf::FloatRect& region, rdr::FactoryTable* factory);
};

} // namespace gui
} // namespace bl

#endif
