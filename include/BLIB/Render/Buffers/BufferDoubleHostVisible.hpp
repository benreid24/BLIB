#ifndef BLIB_RENDER_BUFFERS_BUFFERDOUBLEHOSTVISIBLE_HPP
#define BLIB_RENDER_BUFFERS_BUFFERDOUBLEHOSTVISIBLE_HPP

#include <BLIB/Render/Buffers/Interfaces/BindableBufferDouble.hpp>
#include <BLIB/Render/Buffers/Role.hpp>

namespace bl
{
namespace rc
{
namespace buf
{

/**
 * @brief Double buffered host visible buffer with no source buffer. Intended for
 *        use with buffers that get fully rewritten every frame

* * @tparam T The type of data in the buffer
 * @ingroup Renderer
 */
template<typename T, Role TRole>
using BufferDoubleHostVisible = base::BindableBufferDouble<
    T, RoleInfo<TRole>::DataAlignment,
    RoleInfo<TRole>::Usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
    VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT>;

/**
 * @brief Helper define for the SSBO role
 *
 * @tparam T The type of data in the buffer
 * @ingroup Renderer
 */
template<typename T>
using BufferDoubleHostVisibleSSBO = BufferDoubleHostVisible<T, Role::SSBO>;

/**
 * @brief Helper define for the UBO role
 *
 * @tparam T The type of data in the buffer
 * @ingroup Renderer
 */
template<typename T>
using BufferDoubleHostVisibleUBO = BufferDoubleHostVisible<T, Role::UBO>;

} // namespace buf
} // namespace rc
} // namespace bl

#endif
