#ifndef BLIB_MENU_EVENTGENERATORS_MOUSEEVENTGENERATOR_HPP
#define BLIB_MENU_EVENTGENERATORS_MOUSEEVENTGENERATOR_HPP

#include <BENG/Menu/Item.hpp>
#include <BENG/Menu/Menu.hpp>
#include <BENG/Util/EventListener.hpp>
#include <SFML/Window/Mouse.hpp>

namespace bg
{
namespace menu
{
/**
 * @brief Basic helper class to map mouse input to menu events
 *
 * @ingroup Menu
 *
 */
class MouseEventGenerator : public bg::WindowEventListener {
public:
    /**
     * @brief Create a new mouse generator that services the given menu
     *
     * @param menu Menu to feed events into. Must remain valid for the lifetime of this object
     * @param allowClickActivate True to allow the mouse button click to send an activate event
     * @param activateButton The mouse button that sends activate events
     */
    MouseEventGenerator(Menu& menu, bool allowClickActivate = true,
                        sf::Mouse::Button activateButton = sf::Mouse::Left);

    /**
     * @brief Set the mouse button bound to activate events
     *
     */
    void setMouseActivateButton(sf::Mouse::Button activateButton);

    /**
     * @brief Sets whether or not clicking the mouse sends activation events
     *
     */
    void setAllowButtonActivate(bool allow);

    virtual ~MouseEventGenerator() = default;

private:
    Menu& menu;
    bool clickActivates;
    sf::Mouse::Button activateButton;

    virtual void observe(const sf::Event& event) override;
};

} // namespace menu
} // namespace bg

#endif
