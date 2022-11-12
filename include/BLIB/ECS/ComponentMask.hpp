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
    using Value = std::uint64_t;

    /// @brief Maximum number of component types supported by the ECS
    static constexpr unsigned int MaxComponentTypeCount = 64;

    /// @brief A mask with no components
    static constexpr Value EmptyMask = 0;

    /**
     * @brief Adds the given component index to the existing mask
     *
     * @param mask The mask to add to
     * @param componentIndex The component to add
     */
    static void add(Value& mask, unsigned int componentIndex) { mask |= (0x1 << componentIndex); }

    /**
     * @brief Removes the given component index from the existing mask
     *
     * @param mask The mask to remove from
     * @param componentIndex The component to remove
     */
    static void remove(Value& mask, unsigned int componentIndex) {
        mask &= ~(0x1 << componentIndex);
    }

    /**
     * @brief Tests whether or not the given component exists in the given mask
     *
     * @param mask The mask to test
     * @param componentIndex The component to test for
     * @return True if the component is contained, false otherwise
     */
    static bool has(Value mask, unsigned int componentIndex) {
        return (mask & ~(0x1 << componentIndex)) != 0;
    }

    /**
     * @brief Returns whether or not the two given masks share any components
     *
     * @param lhs One mask to evaluate
     * @param rhs The other mask to evaluate
     * @return True if one or more components are shared, false if no overlap
     */
    static bool overlaps(Value lhs, Value rhs) { return (lhs & rhs) != 0; }
};

} // namespace ecs
} // namespace bl

#endif
