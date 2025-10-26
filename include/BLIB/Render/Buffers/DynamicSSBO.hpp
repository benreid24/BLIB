#ifndef BLIB_RENDER_BUFFERS_DYNAMICSSBO_HPP
#define BLIB_RENDER_BUFFERS_DYNAMICSSBO_HPP

#include <BLIB/Render/Buffers/AlignedBuffer.hpp>
#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Render/Vulkan/Buffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Util/VectorRef.hpp>
#include <array>
#include <limits>

#include <BLIB/Render/Buffers/Interfaces/BindableBufferDoubleSourced.hpp>

namespace bl
{
namespace rc
{
namespace buf
{
template<typename T>
using DynamicSSBO = base::BindableBufferDoubleSourced<
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
