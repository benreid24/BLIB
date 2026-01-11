#ifndef BLIB_RENDER_BUFFERS_BUFFERSINGLEDEVICELOCALSOURCED_HPP
#define BLIB_RENDER_BUFFERS_BUFFERSINGLEDEVICELOCALSOURCED_HPP

#include <BLIB/Render/Buffers/AlignedBuffer.hpp>
#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Render/Vulkan/Buffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>

#include <BLIB/Render/Buffers/Interfaces/BindableBufferSingleSourced.hpp>
#include <BLIB/Render/Buffers/Role.hpp>

namespace bl
{
namespace rc
{
namespace buf
{
/**
 * @brief Single buffered device local buffer with host source buffer and lazily created staging
 *        buffers on transfer
 *
 * @tparam T The data type to store in the buffer
 * @tparam TRole The role of the buffer
 * @ingroup Renderer
 */
template<typename T, Role TRole>
using BufferSingleDeviceLocalSourced =
    base::BindableBufferSingleSourced<T, RoleInfo<TRole>::DataAlignment,
                                      RoleInfo<TRole>::Usage | VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                                          VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT>;

/**
 * @brief Specialization of BufferSingleDeviceLocalSourced for SSBO's
 *
 * @tparam T The type of data to store
 * @ingroup Renderer
 */
template<typename T>
using BufferSingleDeviceLocalSourcedSSBO = BufferSingleDeviceLocalSourced<T, Role::SSBO>;

/**
 * @brief Specialization of BufferSingleDeviceLocalSourced for UBO's
 *
 * @tparam T The type of data to store
 * @ingroup Renderer
 */
template<typename T>
using BufferSingleDeviceLocalSourcedUBO = BufferSingleDeviceLocalSourced<T, Role::UBO>;

} // namespace buf
} // namespace rc
} // namespace bl

#endif
