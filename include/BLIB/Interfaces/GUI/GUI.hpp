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
     * @param world The world to create entities in
     * @param player The player the GUI will belong to
     * @param packer The Packer to use
     * @param region The position and size of the renderable area. Defaults to the full overlay
     *               space unless a value is manually set
     * @param factory The component factory to use in the renderer. Must remain valid
     */
    static Ptr create(engine::World& world, engine::Player& player, const gui::Packer::Ptr& packer,
                      const sf::FloatRect& region = {}, rdr::FactoryTable* factory = nullptr);

    /**
     * @brief Set the Region objectSets the region to pack elements into
     *
     * @param area The new region to fill
     */
    void setRegion(const sf::FloatRect& area);

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
     * @param autoSubscribe Whether to directly subscribe to window events
     */
    void addToOverlay(rc::Overlay* overlay = nullptr, bool autoSubscribe = true);

    /**
     * @brief Removes the GUI and all elements from the current overlay
     */
    void removeFromScene();

    /**
     * @brief Returns the last known mouse position in overlay space
     */
    const glm::vec2& getMousePosition() const { return mousePos; }

    /**
     * @brief Processes the window event and returns whether the event was consumed. This does not
     *        need to be manually called unless autoSubscribe is turned off in addToOverlay
     *
     * @param event The event to process
     * @return True if the event had effect, false otherwise
     */
    bool processEvent(const sf::Event& event);

private:
    rc::Observer& observer;
    std::vector<Element::QueuedAction> queuedActions;
    rdr::Renderer renderer;
    glm::vec2 mousePos;

    GUI(engine::World& world, engine::Player& player, const gui::Packer::Ptr& packer,
        const sf::FloatRect& region, rdr::FactoryTable* factory);
    virtual void observe(const sf::Event& event) override;
};

} // namespace gui
} // namespace bl

#endif
