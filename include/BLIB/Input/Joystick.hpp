#ifndef BLIB_INPUT_JOYSTICK_HPP
#define BLIB_INPUT_JOYSTICK_HPP

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Joystick.hpp>

namespace bl
{
namespace input
{
/**
 * @brief Helper struct representing a full joystick for inputs
 *
 * @ingroup Input
 *
 */
struct Joystick {
    sf::Joystick::Axis verticalAxis;
    sf::Joystick::Axis horizontalAxis;
    bool verticalInverted;
    bool horizontalInverted;
    sf::Vector2f cachedPosition; // not normalized

    /**
     * @brief Construct a new Joystick with sane defaults
     *
     */
    Joystick();

    /**
     * @brief Reads the joystick value into a normalized vector
     *
     * @param joystick The index of the joystick to read from
     *
     */
    sf::Vector2f read(unsigned int joystick) const;

    /**
     * @brief Updates the cached position from the given event
     *
     * @param event The event to process
     * @return True if the event had an effect, false otherwise
     */
    bool process(const sf::Event& event);

    /**
     * @brief Returns a user-readable string signifying what this control is
     *
     */
    std::string toString() const;

    /**
     * @brief Converts the given joystick axis to a user readable string
     *
     * @param axis The axis to get the string for
     * @return std::string A user-readable string for the given axis
     */
    static std::string axisToString(sf::Joystick::Axis axis);

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
