#ifndef BLIB_INPUT_DISPATCHTYPE_HPP
#define BLIB_INPUT_DISPATCHTYPE_HPP

namespace bl
{
namespace input
{
/**
 * @brief Indicates if a control notification is the first activation of that control or if it is a
 *        repeated signal (ie sent every update loop while active). Also used for breaking down
 *        movement control events into directions usable by Menus
 *
 * @ingroup Input
 *
 */
enum struct DispatchType {
    /// @brief Used internally only
    _INVALID,

    /// @brief The a trigger control was activated by an event
    TriggerActivated,

    /// @brief The directional control changed values
    DirectionChanged,

    /// @brief Movement control was activated in the up/forward direction
    MovementUp,

    /// @brief Movement control was activated in the right direction
    MovementRight,

    /// @brief Movement control was activated in the down/backward direction
    MovementDown,

    /// @brief Movement control was activated in the left direction
    MovementLeft
};

} // namespace input
} // namespace bl

#endif
