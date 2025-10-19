#ifndef BLIB_RENDER_BUFFERS_ALIGNMENT_HPP
#define BLIB_RENDER_BUFFERS_ALIGNMENT_HPP

#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace buf
{
/**
 * @brief Basic enum representing the different alignment requirements certain buffers can satisfy
 *
 * @ingroup Renderer
 */
enum struct Alignment {
    /// Data is packed in with no extra padding
    Packed,

    /// The data is treated as an array in std140 layout and aligned as such
    Std140,

    /// The data is treated as an array in std430 layout and aligned as such
    Std430,

    /// The data is treated as an array with data aligned to UBO rules for bind offset
    UboBindOffset,

    /// The data is treated as an array with data aligned to SSBO rules for bind offset
    SsboBindOffset
};

/**
 * @brief Helper to compute data alignment for a certain block size and alignment requirement
 *
 * @param dataSize The size of the data to align
 * @param alignment The alignment requirement to satisfy
 * @return The aligned size of the data
 */
VkDeviceSize computeAlignment(VkDeviceSize dataSize, Alignment alignment);

} // namespace buf
} // namespace rc
} // namespace bl

#endif
