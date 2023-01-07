#ifndef BLIB_RENDER_RENDERER_OBJECTFLAGS_HPP
#define BLIB_RENDER_RENDERER_OBJECTFLAGS_HPP

#include <cstdint>

namespace bl
{
namespace render
{
/**
 * @brief Helper struct to represent dirty states an object may end up in
 *
 * @ingroup Renderer
 */
class ObjectFlags {
public:
    /**
     * @brief Construct new Object Flags with no flags set
     *
     */
    ObjectFlags();

    /**
     * @brief Marks the object's push constants as needing to be refreshed
     *
     */
    void markPCDirty();

    /**
     * @brief Returns whether or not any dirty flags are set
     *
     * @return True if any flag is set, false if none are set
     */
    constexpr bool isDirty() const;

    /**
     * @brief Returns whether or not the object's push constant flag is set
     *
     * @return True if the push constants need to be refreshed, false otherwise
     */
    constexpr bool isPCDirty() const;

    /**
     * @brief Resets all of the flags to false
     *
     */
    void reset();

private:
    std::uint8_t value;

    static constexpr std::uint8_t PC = 0x1 << 0;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline ObjectFlags::ObjectFlags()
: value(0) {}

inline void ObjectFlags::markPCDirty() { value |= PC; }

inline constexpr bool ObjectFlags::isPCDirty() const { return value & PC != 0; }

inline void ObjectFlags::reset() { value = 0; }

} // namespace render
} // namespace bl

#endif
