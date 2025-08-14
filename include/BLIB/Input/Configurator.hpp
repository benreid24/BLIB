#ifndef BLIB_INPUT_CONFIGURATOR_HPP
#define BLIB_INPUT_CONFIGURATOR_HPP

#include <BLIB/Input/Joystick.hpp>
#include <BLIB/Input/Trigger.hpp>
#include <BLIB/Signals/Listener.hpp>
#include <SFML/Window/Event.hpp>

namespace bl
{
namespace engine
{
class Engine;
}
namespace input
{
/**
 * @brief Helper to set controls from window events
 *
 * @ingroup Input
 */
class Configurator : public sig::Listener<sf::Event> {
public:
    enum State { Finished, WaitingTrigger, WaitingHorAxis, WaitingVertAxis };

    /**
     * @brief Construct a new Configurator object
     */
    Configurator();

    /**
     * @brief Start the configurator for the given control
     *
     * @param engine The game engine instance
     * @param toSet The control to set
     */
    void start(engine::Engine& engine, Trigger& toSet);

    /**
     * @brief Start the configurator for the given control
     *
     * @param engine The game engine instance
     * @param toSet The joystick to set
     */
    void start(engine::Engine& engine, Joystick& stick);

    /**
     * @brief Returns whether the Configurator is finished or not
     */
    bool finished() const;

    /**
     * @brief Returns a more fine-grained state than just finished()
     */
    State getState() const;

private:
    enum struct JoystickState { WaitingPositive, WaitingNegative };

    State state;
    union {
        Trigger* trigger;
        Joystick* joystick;
    };
    JoystickState jsState;
    sf::Joystick::Axis axis;

    virtual void process(const sf::Event& event) override;
};

} // namespace input
} // namespace bl

#endif
