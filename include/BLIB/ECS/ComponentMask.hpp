#ifndef BLIB_ECS_COMPONENTMASK_HPP
#define BLIB_ECS_COMPONENTMASK_HPP

#include <cstdint>

namespace bl
{
namespace ecs
{
class Registry;

/**
 * @brief Helper struct for creating masks representing component types on an entity
 *
 * @ingroup ECS
 *
 */
struct ComponentMask {
    /// @brief The type of the bitmask itself
    using SimpleMask = BLIB_ECS_MASK_TYPE;

    /// @brief Maximum number of component types supported by the ECS
    static constexpr unsigned int MaxComponentTypeCount = sizeof(SimpleMask) * 8;

    /// @brief A mask with no components
    static constexpr SimpleMask EmptyMask = 0;

    SimpleMask required = 0;
    SimpleMask optional = 0;
    SimpleMask excluded = 0;

    /**
     * @brief Returns whether or not the given mask passes the tagged filter for this mask
     *
     * @param entityMask The mask to check
     * @return True if the mask satisfies the tagged requirements, false otherwise
     */
    bool passes(SimpleMask entityMask) const {
        return (entityMask & required) == required && (entityMask & excluded) == 0;
    }

    /**
     * @brief Returns whether or not this mask contains the given index. Uses both required and
     *        optional masks
     *
     * @param index The index to check for
     * @return True if the given index is contained, false otherwise
     */
    bool contains(unsigned int index) const {
        return ((required | optional) & (static_cast<SimpleMask>(0x1) << index)) != 0;
    }

    /**
     * @brief Returns whether or not this mask contains the given index
     *
     * @param mask The mask to test
     * @param index The index to check for
     * @return True if the given index is contained, false otherwise
     */
    static bool contains(SimpleMask mask, unsigned int index) {
        return (mask & (static_cast<SimpleMask>(0x1) << index)) != 0;
    }

    /**
     * @brief Adds the given component index to the existing mask
     *
     * @param mask The mask to add to
     * @param componentIndex The component to add
     */
    static void add(SimpleMask& mask, unsigned int componentIndex) {
        mask |= (static_cast<SimpleMask>(0x1) << componentIndex);
    }

    /**
     * @brief Removes the given component index from the existing mask
     *
     * @param mask The mask to remove from
     * @param componentIndex The component to remove
     */
    static void remove(SimpleMask& mask, unsigned int componentIndex) {
        mask &= ~(static_cast<SimpleMask>(0x1) << componentIndex);
    }

    /**
     * @brief Tests whether or not the given component exists in the given mask
     *
     * @param mask The mask to test
     * @param componentIndex The component to test for
     * @return True if the component is contained, false otherwise
     */
    static bool has(SimpleMask mask, unsigned int componentIndex) {
        return (mask & (static_cast<SimpleMask>(0x1) << componentIndex)) != 0;
    }
};

} // namespace ecs
} // namespace bl

#endif
