#ifndef BLIB_INTERFACES_MENU_DRIVERS_MOVEMENTDRIVER_HPP
#define BLIB_INTERFACES_MENU_DRIVERS_MOVEMENTDRIVER_HPP

#include <BLIB/Input/Listener.hpp>
#include <BLIB/Interfaces/Menu/Menu.hpp>

namespace bl
{
namespace menu
{
/**
 * @brief Helper class to receive user input from the InputSystem and drive a menu with it. Must be
 *        subscribed to an input::Actor to work
 *
 * @tparam MovementControl The control index of the MovementControl to move on
 * @tparam ActivateTrigger The control index of the Trigger control to activate on
 *
 * @ingroup Menu
 * @ingroup Input
 */
template<unsigned int MovementControl, unsigned int ActivateTrigger>
class MovementDriver : public input::Listener {
public:
    /// @brief The default debounce time for held inputs
    static constexpr float DefaultDebounce = 0.5f;

    /**
     * @brief Construct a new Movement Driver with no menu to be driven
     *
     */
    MovementDriver()
    : driving(nullptr)
    , debounceTime(DefaultDebounce) {}

    /**
     * @brief Construct a new Movement Driver to drive the given menu
     *
     * @param toDrive
     */
    MovementDriver(Menu& toDrive)
    : driving(&toDrive)
    , debounceTime(DefaultDebounce) {}

    /**
     * @brief Change the menu being driven to the given one
     *
     * @param toDrive The menu to drive, or nullptr to disconnect
     */
    void drive(Menu& toDrive) { driving = &toDrive; }

    /**
     * @brief Set the debounce time between repeated input events not triggered by user input
     *
     * @param debounceTime The time between repeated events, in seconds
     */
    void setDebounce(float debounceTime) { this->debounceTime = debounceTime; }

    /**
     * @brief Programmatically sends the given control to the menu
     *
     * @param ctrl The control to send
     * @param moveDir The move direction if sending a move control
     * @param ignoreDebounce True to ignore the signal cooldown and send always, false to debounce
     * @return True if the input was processed, false if it did not get sent to a menu
     */
    bool sendControl(unsigned int ctrl, input::DispatchType moveDir, bool ignoreDebounce) {
        if (driving != nullptr) {
            // enforce debounce on repeat events (not from user events)
            if (!ignoreDebounce && debounce.getElapsedTime().asSeconds() < debounceTime)
                return false;
            if (!ignoreDebounce) debounce.restart();

            if (ctrl == MovementControl) {
                switch (moveDir) {
                case input::DispatchType::MovementUp:
                    driving->processEvent(Event(Event::MoveEvent(Item::Top)));
                    return true;
                case input::DispatchType::MovementRight:
                    driving->processEvent(Event(Event::MoveEvent(Item::Right)));
                    return true;
                case input::DispatchType::MovementDown:
                    driving->processEvent(Event(Event::MoveEvent(Item::Bottom)));
                    return true;
                case input::DispatchType::MovementLeft:
                    driving->processEvent(Event(Event::MoveEvent(Item::Left)));
                    return true;
                default:
                    break;
                }
            }
            else if (ctrl == ActivateTrigger) {
                driving->processEvent(Event(Event::ActivateEvent()));
                return true;
            }
        }
        return false;
    }

private:
    Menu* driving;
    sf::Clock debounce;
    float debounceTime;

    virtual bool observe(const input::Actor&, unsigned int activatedControl,
                         input::DispatchType eventType, bool eventTriggered) override {
        return sendControl(activatedControl, eventType, eventTriggered);
    }
};

} // namespace menu
} // namespace bl

#endif
