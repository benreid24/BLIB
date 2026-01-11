#ifndef BLIB_MENU_DRIVERS_KEYBOARDDRIVER_HPP
#define BLIB_MENU_DRIVERS_KEYBOARDDRIVER_HPP

#include <BLIB/Interfaces/Menu/Item.hpp>
#include <BLIB/Interfaces/Menu/Menu.hpp>
#include <BLIB/Signals/Listener.hpp>
#include <SFML/Window.hpp>

namespace bl
{
namespace menu
{
/**
 * @brief Basic helper class to map keyboard input to menu events
 *
 * @ingroup Menu
 *
 */
class KeyboardDriver : public bl::sig::Listener<sf::Event> {
public:
    /**
     * @brief Create a new keyboard generator that services the given menu
     *
     * @param menu Menu to feed events into. Must remain valid for the lifetime of this object
     * @param upCtrl Control to generate move up events
     * @param rightCtrl Control to generate move right events
     * @param downCtrl Control to generate move down events
     * @param leftCtrl Control to generate move left events
     * @param activateCtrl Control to generate activate events
     */
    KeyboardDriver(Menu& menu, sf::Keyboard::Key upCtrl = sf::Keyboard::Up,
                   sf::Keyboard::Key rightCtrl    = sf::Keyboard::Right,
                   sf::Keyboard::Key downCtrl     = sf::Keyboard::Down,
                   sf::Keyboard::Key leftCtrl     = sf::Keyboard::Left,
                   sf::Keyboard::Key activateCtrl = sf::Keyboard::Space);

    /**
     * @brief Set the key bound to the given direction. Defaults are the arrow keys
     *
     * @param direction The control direction to bind to
     * @param control The key to bind
     */
    void setMoveControl(Item::AttachPoint direction, sf::Keyboard::Key control);

    /**
     * @brief Set the key that is bound to the activate event. Default is space bar
     *
     * @param control The key that should generator activate events
     */
    void setActivateControl(sf::Keyboard::Key control);

    /**
     * @brief Destroy the Keyboard Driver object
     *
     */
    virtual ~KeyboardDriver() = default;

    /**
     * @brief Processes the given window event and drives the menu if relevant
     *
     * @param event The window event to process
     */
    virtual void process(const sf::Event& event) override;

private:
    Menu& menu;
    sf::Keyboard::Key moveControls[Item::AttachPoint::_NUM_ATTACHPOINTS];
    sf::Keyboard::Key activateControl;
};

} // namespace menu
} // namespace bl

#endif
