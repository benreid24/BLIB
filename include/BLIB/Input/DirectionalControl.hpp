#ifndef BLIB_INPUT_DIRECTIONALCONTROL_HPP
#define BLIB_INPUT_DIRECTIONALCONTROL_HPP

#include <BLIB/Input/Joystick.hpp>

namespace bl
{
namespace input
{
class InputSystem;

/**
 * @brief Control data for a directional input
 *
 * @ingroup Input
 *
 */
struct DirectionalControl {
    /// @brief The type of input that drives the control
    enum struct Type { Mouse, Joystick };

    Type type;
    union {
        Joystick joystick;
    };
    sf::Vector2f normalizedDirection;

    /**
     * @brief Defaults to mouse control
     *
     */
    DirectionalControl();

    /**
     * @brief Processes the given event to potentially update the direction being inputted
     *
     * @param system The InputSystem itself to read the mouse vector from
     * @param event The event to process
     * @return True if the direction changed, false otherwise
     */
    bool process(const InputSystem& system, const sf::Event& event);

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
