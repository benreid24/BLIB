#ifndef BLIB_INPUT_CONFIGURATOR_HPP
#define BLIB_INPUT_CONFIGURATOR_HPP

#include <BLIB/Events/Dispatcher.hpp>
#include <BLIB/Events/Listener.hpp>
#include <BLIB/Input/Joystick.hpp>
#include <BLIB/Input/Trigger.hpp>
#include <SFML/Window/Event.hpp>

namespace bl
{
namespace input
{
/**
 * @brief Helper to set controls from window events
 *
 * @ingroup Input
 *
 */
class Configurator : public event::Listener<sf::Event> {
public:
    enum State { Finished, WaitingTrigger, WaitingHorAxis, WaitingVertAxis };

    /**
     * @brief Construct a new Configurator object
     *
     * @param bus The event bus to use for configuration events
     */
    Configurator(event::Dispatcher& bus);

    /**
     * @brief Start the configurator for the given control
     *
     * @param toSet The control to set
     */
    void start(Trigger& toSet);

    /**
     * @brief Start the configurator for the given control
     *
     * @param toSet The joystick to set
     */
    void start(Joystick& stick);

    /**
     * @brief Returns whether the Configurator is finished or not
     *
     */
    bool finished() const;

    /**
     * @brief Returns a more fine-grained state than just finished()
     *
     */
    State getState() const;

private:
    enum struct JoystickState { WaitingPositive, WaitingNegative };

    State state;
    event::Dispatcher& bus;
    union {
        Trigger* trigger;
        Joystick* joystick;
    };
    JoystickState jsState;
    sf::Joystick::Axis axis;

    virtual void observe(const sf::Event& event) override;
};

} // namespace input
} // namespace bl

#endif