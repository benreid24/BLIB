#ifndef BLIB_RENDER_BUFFERS_BUFFERDOUBLESTAGED_HPP
#define BLIB_RENDER_BUFFERS_BUFFERDOUBLESTAGED_HPP

#include <BLIB/Render/Buffers/Interfaces/BindableBufferDoubleStaged.hpp>
#include <BLIB/Render/Buffers/Role.hpp>

namespace bl
{
namespace rc
{
/// Collection of common buffer classes
namespace buf
{
/**
 * @brief Generic buffer for double buffered persistently staged buffers that get fully rewritten
 *        every frame
 *
 * @tparam T The type of data in the buffer
 * @tparam TRole The role of the buffer
 * @ingroup Renderer
 */
template<typename T, Role TRole>
using BufferDoubleStaged =
    base::BindableBufferDoubleStaged<T, RoleInfo<TRole>::DataAlignment,
                                     RoleInfo<TRole>::Usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT>;

/**
 * @brief Specialization of BufferDoubleStaged for the UBO role
 *
 * @tparam T The type of data in the buffer
 * @ingroup Renderer
 */
template<typename T>
using BufferDoubleStagedUBO = BufferDoubleStaged<T, Role::UBO>;

} // namespace buf
} // namespace rc
} // namespace bl

#endif
