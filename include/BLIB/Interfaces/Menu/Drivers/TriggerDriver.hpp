#ifndef BLIB_INTERFACES_MENU_DRIVERS_TRIGGERDRIVER_HPP
#define BLIB_INTERFACES_MENU_DRIVERS_TRIGGERDRIVER_HPP

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
 * @tparam UpTrigger The Trigger control to move up on
 * @tparam RightTrigger The Trigger control to move right on
 * @tparam DownTrigger The Trigger control to move down on
 * @tparam LeftTrigger The Trigger control to move left on
 * @tparam ActivateTrigger The Trigger control to activate on
 *
 * @ingroup Menu
 * @ingroup Input
 */
template<unsigned int UpTrigger, unsigned int RightTrigger, unsigned int DownTrigger,
         unsigned int LeftTrigger, unsigned int ActivateTrigger>
class TriggerDriver : public input::Listener {
public:
    /// @brief The default debounce time for held inputs
    static constexpr float DefaultDebounce = 0.5f;

    /**
     * @brief Construct a new Trigger Driver with no menu to be driven
     *
     */
    TriggerDriver()
    : driving(nullptr)
    , debounceTime(DefaultDebounce) {}

    /**
     * @brief Construct a new Trigger Driver to drive the given menu
     *
     * @param toDrive
     */
    TriggerDriver(Menu& toDrive)
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
     * @param ignoreDebounce True to ignore the signal cooldown and send always, false to debounce
     * @return True if the input was processed, false if it did not get sent to a menu
     */
    bool sendControl(unsigned int ctrl, bool ignoreDebounce) {
        if (driving != nullptr) {
            // enforce debounce on repeat events (not from user events)
            if (!ignoreDebounce && debounce.getElapsedTime().asSeconds() < debounceTime)
                return false;
            if (!ignoreDebounce) debounce.restart();

            switch (ctrl) {
            case UpTrigger:
                driving->processEvent(Event(Event::MoveEvent(Item::Top)));
                return true;
            case RightTrigger:
                driving->processEvent(Event(Event::MoveEvent(Item::Right)));
                return true;
            case DownTrigger:
                driving->processEvent(Event(Event::MoveEvent(Item::Bottom)));
                return true;
            case LeftTrigger:
                driving->processEvent(Event(Event::MoveEvent(Item::Left)));
                return true;
            case ActivateTrigger:
                driving->processEvent(Event(Event::ActivateEvent()));
                return true;
            default:
                break;
            }
        }
        return false;
    }

private:
    Menu* driving;
    sf::Clock debounce;
    float debounceTime;

    virtual void observe(const input::Actor&, unsigned int activatedControl, input::DispatchType,
                         bool eventTriggered) override {
        return sendControl(activatedControl, eventTriggered);
    }
};

} // namespace menu
} // namespace bl

#endif
