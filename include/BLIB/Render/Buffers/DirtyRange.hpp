#ifndef BLIB_RENDER_BUFFERS_DIRTYRANGE_HPP
#define BLIB_RENDER_BUFFERS_DIRTYRANGE_HPP

#include <cstdint>

namespace bl
{
namespace rc
{
namespace buf
{
/**
 * @brief Basic helper struct to track dirty ranges of buffers
 *
 * @ingroup Renderer
 */
struct DirtyRange {
    std::uint32_t start;
    std::uint32_t size;

    /**
     * @brief Creates an empty dirty range
     */
    DirtyRange();

    /**
     * @brief Creates a dirty range from the given indices
     *
     * @param start The start index of the dirty range
     * @param n The number of dirty elements
     */
    DirtyRange(std::uint32_t start, std::uint32_t n = 1);

    /**
     * @brief Combines this range to include the union of itself and another range
     *
     * @param other The range to combine with
     */
    void combine(const DirtyRange& other);

    /**
     * @brief Resets the range to be empty
     */
    void reset();

    /**
     * @brief Marks the given range as dirty, expanding this range to include itself and it
     *
     * @param i The first index of the dirty element
     * @param n The number of dirty elements
     */
    void markDirty(std::uint32_t i, std::uint32_t n = 1);
};

} // namespace buf
} // namespace rc
} // namespace bl

#endif
