#ifndef BLIB_GUI_GUI_HPP
#define BLIB_GUI_GUI_HPP

#include <BLIB/GUI/Elements/Container.hpp>
#include <BLIB/Util/EventDispatcher.hpp>
#include <BLIB/Util/EventListener.hpp>
#include <SFML/Graphics.hpp>

namespace bl
{
/**
 * @brief Top level class for creating a graphical user interface
 *
 * A GUI object is what client code must use for manipulating and rendering an interface. A GUI
 * object may only be used in a single sf::RenderWindow, but an sf::RenderWindow object may
 * contain multiple GUI's. A GUI may be positioned and have a size set. Elements in the GUI
 * will be positioned and constrained by the GUI's position and size.
 *
 */
class GUI
: public sf::Drawable
, public bl::WindowEventListener
, private gui::Container {
public:
    /**
     * @brief Create a new GUI that is subscribed to the dispatcher and renders to the given
     *        region
     *
     * @param dispatcher The dispatcher to subscribe to
     * @param region The position and size of the renderable area
     */
    GUI(bl::WindowEventDispatcher& dispatcher, const sf::IntRect& region);

    /**
     * @brief Create a new GUI that is subscribed to a dispatcher and fills the window
     *
     * @param dispatcher The dispatcher to subscribe to
     * @param window The window to fill
     */
    GUI(bl::WindowEventDispatcher& dispatcher, const sf::RenderWindow& window);

    /**
     * @brief Handles and propogates the window event
     *
     * @param event Raw window event
     */
    virtual void observe(const sf::Event& event) override;

    // TODO - use Box for the region. Have add() method for windows to float. Can windows be
    // children?

protected:
    /**
     * @brief Renders the GUI to the target
     *
     * @param target Target to render to
     */
    virtual void draw(sf::RenderTarget& target, sf::RenderStates) const override;

    /**
     * @brief Brings the child Element on top
     *
     * @param child The element to raise
     */
    virtual void bringToTop(const Element* child) override;
};

} // namespace bl

#endif