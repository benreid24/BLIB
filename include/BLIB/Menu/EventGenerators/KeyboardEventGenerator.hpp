#ifndef BLIB_MENU_EVENTGENERATORS_KEYBOARDEVENTGENERATOR_HPP
#define BLIB_MENU_EVENTGENERATORS_KEYBOARDEVENTGENERATOR_HPP

#include <BLIB/Menu/Item.hpp>
#include <BLIB/Menu/Menu.hpp>
#include <BLIB/Events/EventListener.hpp>
#include <SFML/Window/Keyboard.hpp>

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
class KeyboardEventGenerator : public bl::WindowEventListener {
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
    KeyboardEventGenerator(Menu& menu, sf::Keyboard::Key upCtrl = sf::Keyboard::Up,
                           sf::Keyboard::Key rightCtrl    = sf::Keyboard::Right,
                           sf::Keyboard::Key downCtrl     = sf::Keyboard::Down,
                           sf::Keyboard::Key leftCtrl     = sf::Keyboard::Left,
                           sf::Keyboard::Key activateCtrl = sf::Keyboard::Space);

    /**
     * @brief Set the key bound to the given direction. Defaults are the arrrow keys
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

    virtual ~KeyboardEventGenerator() = default;

private:
    Menu& menu;
    sf::Keyboard::Key moveControls[Item::AttachPoint::_NUM_ATTACHPOINTS];
    sf::Keyboard::Key activateControl;

    virtual void observe(const sf::Event& event) override;
};

} // namespace menu
} // namespace bl

#endif
