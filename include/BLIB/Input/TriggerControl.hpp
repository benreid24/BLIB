#ifndef BLIB_INPUT_TRIGGERCONTROL_HPP
#define BLIB_INPUT_TRIGGERCONTROL_HPP

#include <BLIB/Input/DispatchPolicy.hpp>
#include <BLIB/Input/Trigger.hpp>

namespace bl
{
namespace input
{
/**
 * @brief Control data for a control that triggers a boolean active state
 *
 * @ingroup Input
 *
 */
struct TriggerControl {
    DispatchPolicy policy;
    Trigger trigger;

    /**
     * @brief Defaults to DispatchEachFrame policy
     * 
     */
    TriggerControl();
};

} // namespace input
} // namespace bl

#endif
