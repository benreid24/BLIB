#ifndef BLIB_INPUT_DISPATCHPOLICY_HPP
#define BLIB_INPUT_DISPATCHPOLICY_HPP

namespace bl
{
namespace input
{
/**
 * @brief Various rules dictating when triggered control events should be sent to listeners
 *
 * @ingroup Input
 *
 */
enum struct DispatchPolicy {
    /// Do not dispatch control state to listeners
    NoDispatch,

    /// Dispatch control activation once when it occurs
    DispatchOnTrigger,

    /// Dispatch activated control state continuously
    DispatchEachFrame,

    /// Dispatches each frame, but only for the most recently activated control with this setting
    DispatchDedupedGroup
};

} // namespace input
} // namespace bl

#endif
