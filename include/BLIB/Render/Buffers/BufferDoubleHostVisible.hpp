#ifndef BLIB_RENDER_BUFFERS_FULLYDYNAMICSSBO_HPP
#define BLIB_RENDER_BUFFERS_FULLYDYNAMICSSBO_HPP

#include <BLIB/Render/Buffers/AlignedBuffer.hpp>
#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Render/Vulkan/Buffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Util/VectorRef.hpp>
#include <array>
#include <limits>

#include <BLIB/Render/Buffers/Interfaces/BindableBufferDouble.hpp>

namespace bl
{
namespace rc
{
namespace buf
{

/**
 * @brief Double buffered host visible buffer with no source buffer. Intended for
 *        use as an SSBO that gets fully rewritten every frame

* * @tparam T The type of data in the buffer
 * @ingroup Renderer
 */
template<typename T>
using BufferDoubleHostVisible = base::BindableBufferDouble<
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
