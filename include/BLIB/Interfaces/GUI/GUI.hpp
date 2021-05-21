#ifndef BLIB_GUI_GUI_HPP
#define BLIB_GUI_GUI_HPP

#include <BLIB/Events/Dispatcher.hpp>
#include <BLIB/Events/Listener.hpp>
#include <BLIB/Interfaces/GUI/Elements/Box.hpp>
#include <SFML/Graphics.hpp>

namespace bl
{
/// A window based graphical user interface
namespace gui
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
, public sf::Transformable
, public bl::event::Listener<sf::Event>
, public gui::Box {
public:
    typedef std::shared_ptr<GUI> Ptr;

    /**
     * @brief Create a new GUI that is in the given region
     *
     * @param region The position and size of the renderable area
     */
    static Ptr create(gui::Packer::Ptr packer, const sf::IntRect& region,
                      const std::string& group = "", const std::string& id = "");

    /**
     * @brief Create a new GUI that fills the window
     *
     * @param packer The Packer to use
     * @param window The window to fill
     */
    static Ptr create(gui::Packer::Ptr packer, const sf::RenderWindow& window,
                      const std::string& group = "", const std::string& id = "");

    /**
     * @brief Sets the renderer to use. Default is gui::Renderer
     *
     * @param renderer The renderer to use
     */
    void setRenderer(gui::Renderer::Ptr renderer);

    /**
     * @brief Handles and propogates the window event
     *
     * @param Packer The Packer to use
     * @param event Raw window event
     */
    virtual void observe(const sf::Event& event) override;

    /**
     * @brief Subscribes the GUI to the event dispatcher to start receiving window events
     *
     * @param dispatcher The event dispatcher to get events from
     */
    void subscribe(event::Dispatcher& dispatcher);

protected:
    /**
     * @brief Renders the GUI to the target
     *
     * @param target Target to render to
     * @param states The render states to use
     */
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    gui::Renderer::Ptr renderer;
    sf::Vector2f mousePos;
    mutable sf::Transform renderTransform;

    GUI(gui::Packer::Ptr packer, const std::string& group, const std::string& id);
    GUI(gui::Packer::Ptr packer, const sf::IntRect& region, const std::string& group = "",
        const std::string& id = "");
    GUI(gui::Packer::Ptr packer, const sf::RenderWindow& window, const std::string& group = "",
        const std::string& id = "");
};

} // namespace gui
} // namespace bl

#endif
