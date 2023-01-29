#ifndef BLIB_RENDER_RENDERER_OBJECTFLAGS_HPP
#define BLIB_RENDER_RENDERER_OBJECTFLAGS_HPP

#include <cstdint>
#include <limits>

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
     * @brief Construct new SceneObject Flags with no flags set
     *
     */
    ObjectFlags();

    /**
     * @brief Marks the object's push constants as needing to be refreshed
     *
     */
    void markPCDirty();

    /**
     * @brief Marks the object as having its render pass membership needing updating
     *
     */
    void markRenderStagesDirty();

    /**
     * @brief Marks the object as needing to refresh its draw parameters
     *
     */
    void markDrawParamsDirty();

    /**
     * @brief Sets all flags
     *
     */
    void markAllDirty();

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
     * @brief Returns whether or not this object's render pass membership has changed
     *
     * @return True if the pass batching needs to be redone, false otherwise
     */
    constexpr bool isRenderPassDirty() const;

    /**
     * @brief Returns whether or not this objects draw parameters have changed
     *
     * @return True if the draw parameters need to be refreshed, false otherwise
     */
    constexpr bool isDrawParamsDirty() const;

    /**
     * @brief Resets all of the flags to false
     *
     */
    void reset();

private:
    std::uint8_t value;

    static constexpr std::uint8_t PC = 0x1 << 0;
    static constexpr std::uint8_t RP = 0x1 << 1;
    static constexpr std::uint8_t DP = 0x1 << 2;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline ObjectFlags::ObjectFlags()
: value(0) {}

inline void ObjectFlags::markPCDirty() { value |= PC; }

inline void ObjectFlags::markRenderStagesDirty() { value |= RP; }

inline void ObjectFlags::markDrawParamsDirty() { value |= DP; }

inline void ObjectFlags::markAllDirty() { value = std::numeric_limits<decltype(value)>::max(); }

inline constexpr bool ObjectFlags::isDirty() const { return value != 0; }

inline constexpr bool ObjectFlags::isPCDirty() const { return (value & PC) != 0; }

inline constexpr bool ObjectFlags::isRenderPassDirty() const { return (value & RP) != 0; }

inline constexpr bool ObjectFlags::isDrawParamsDirty() const { return (value & DP) != 0; }

inline void ObjectFlags::reset() { value = 0; }

} // namespace render
} // namespace bl

#endif
