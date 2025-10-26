#ifndef BLIB_RENDER_BUFFERS_BUFFERDOUBLEHOSTVISIBLESOURECED_HPP
#define BLIB_RENDER_BUFFERS_BUFFERDOUBLEHOSTVISIBLESOURECED_HPP

#include <BLIB/Render/Buffers/Interfaces/BindableBufferDoubleSourced.hpp>

namespace bl
{
namespace rc
{
namespace buf
{
/**
 * @brief Double buffered host visible buffer with an aligned host-local source buffer. Intended for
 *        use as an SSBO of frequently changing data but not all data every frame

* * @tparam T The type of data in the buffer
 * @ingroup Renderer
 */
template<typename T>
using BufferDoubleHostVisibleSourced = base::BindableBufferDoubleSourced<
    T, Alignment::Std430,
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
    VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT>;

} // namespace buf
} // namespace rc
} // namespace bl

#endif
