#ifndef BLIB_INPUT_MOVEMENTCONTROL_HPP
#define BLIB_INPUT_MOVEMENTCONTROL_HPP

#include <BLIB/Input/DispatchType.hpp>
#include <BLIB/Input/Joystick.hpp>
#include <BLIB/Input/Trigger.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <stack>
#include <vector>

namespace bl
{
namespace input
{
/**
 * @brief Control data for a movement control (ie WASD or a joystick)
 *
 * @ingroup Input
 *
 */
struct MovementControl {
    /// @brief The type of input that drives the control
    enum struct Type { Joystick, Triggers };

    /// @brief Key data if the keyboard drives the control
    struct KeyConfig {
        Trigger up;
        Trigger right;
        Trigger down;
        Trigger left;

        std::stack<Trigger*, std::vector<Trigger*>> orderStack;
    };

    union {
        Joystick joystick;
        KeyConfig keys;
    };

    /**
     * @brief Defaults to keyboard type with WASD
     *
     */
    MovementControl();

    /**
     * @brief Determines what direction the given event activated, if any
     *
     * @param event The event to process
     * @return DispatchType _INVALID if not a movement, otherwise the direction
     */
    DispatchType process(const sf::Event& event) const;

    /**
     * @brief Saves this control config to the engine configuration store
     * 
     * @param prefix The prefix to use for keys used to save the config
     */
    void saveToConfig(const std::string& prefix) const;

    /**
     * @brief Loads this control config from the engine configuration store
     * 
     * @param prefix The prefix to use for keys to laod from the config
     */
    void loadFromConfig(const std::string& prefix);
};

} // namespace input
} // namespace bl

#endif
