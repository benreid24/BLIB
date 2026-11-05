#ifndef BLIB_INPUT_CONFIGURATION_HPP
#define BLIB_INPUT_CONFIGURATION_HPP

#include <BLIB/Input/Control.hpp>
#include <initializer_list>

namespace bl
{
namespace input
{
/**
 * @brief Combined config for a single control
 *
 * @ingroup Input
 */
struct InputControlConfig {
    unsigned int control;
    Control::Type type;
    Control::Config kbmConfig;
    Control::Config joystickConfig;
};

/**
 * @brief Combined config for all controls in the input system
 *
 * @ingroup Input
 */
struct InputConfiguration {
    std::initializer_list<InputControlConfig> controls;
};

} // namespace input
} // namespace bl

#endif
