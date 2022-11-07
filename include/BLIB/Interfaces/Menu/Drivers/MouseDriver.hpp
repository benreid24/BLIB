#ifndef BLIB_MENU_DRIVERS_MOUSEDRIVER_HPP
#define BLIB_MENU_DRIVERS_MOUSEDRIVER_HPP

#include <BLIB/Events/Listener.hpp>
#include <BLIB/Interfaces/Menu/Item.hpp>
#include <BLIB/Interfaces/Menu/Menu.hpp>
#include <SFML/Window.hpp>

namespace bl
{
namespace menu
{
/**
 * @brief Basic helper class to map mouse input to menu events
 *
 * @ingroup Menu
 *
 */
class MouseDriver : public bl::event::Listener<sf::Event> {
public:
    /**
     * @brief Create a new mouse generator that services the given menu
     *
     * @param menu Menu to feed events into. Must remain valid for the lifetime of this object
     * @param allowClickActivate True to allow the mouse button click to send an activate event
     * @param activateButton The mouse button that sends activate events
     */
    MouseDriver(Menu& menu, bool allowClickActivate = true,
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

    /**
     * @brief Destroy the Mouse Driver object
     *
     */
    virtual ~MouseDriver() = default;

private:
    Menu& menu;
    bool clickActivates;
    sf::Mouse::Button activateButton;

    virtual void observe(const sf::Event& event) override;
};

} // namespace menu
} // namespace bl

#endif
