#ifndef BLIB_ECS_COMPONENTMASK_HPP
#define BLIB_ECS_COMPONENTMASK_HPP

#include <cstdint>

namespace bl
{
namespace ecs
{
/**
 * @brief Helper struct for creating masks representing component types on an entity
 *
 * @ingroup ECS
 *
 */
struct ComponentMask {
    /// @brief The type of the bitmask itself
    using Value = BLIB_ECS_MASK_TYPE;

    /// @brief Maximum number of component types supported by the ECS
    static constexpr unsigned int MaxComponentTypeCount = sizeof(Value) * 8;

    /// @brief A mask with no components
    static constexpr Value EmptyMask = 0;

    /**
     * @brief Adds the given component index to the existing mask
     *
     * @param mask The mask to add to
     * @param componentIndex The component to add
     */
    static void add(Value& mask, unsigned int componentIndex) {
        mask |= (static_cast<Value>(0x1) << componentIndex);
    }

    /**
     * @brief Removes the given component index from the existing mask
     *
     * @param mask The mask to remove from
     * @param componentIndex The component to remove
     */
    static void remove(Value& mask, unsigned int componentIndex) {
        mask &= ~(static_cast<Value>(0x1) << componentIndex);
    }

    /**
     * @brief Tests whether or not the given component exists in the given mask
     *
     * @param mask The mask to test
     * @param componentIndex The component to test for
     * @return True if the component is contained, false otherwise
     */
    static bool has(Value mask, unsigned int componentIndex) {
        return (mask & (static_cast<Value>(0x1) << componentIndex)) != 0;
    }

    /**
     * @brief Returns whether or not the outer mask completely contains the inner mask
     *
     * @param outer The larger mask to test if it contains the smaller mask
     * @param inner The mask to see if is fully contained
     * @return True if the outer mask has all of the components contained in the inner mask
     */
    static bool completelyContains(Value outer, Value inner) { return (outer & inner) == inner; }
};

} // namespace ecs
} // namespace bl

#endif
