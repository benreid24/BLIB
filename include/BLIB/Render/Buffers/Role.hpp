#ifndef BLIB_RENDER_BUFFERS_ROLE_HPP
#define BLIB_RENDER_BUFFERS_ROLE_HPP

#include <BLIB/Render/Buffers/Alignment.hpp>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace buf
{
/**
 * @brief Enum representing roles that generic buffers can be. Affects alignment and usage flags
 *
 * @ingroup Renderer
 */
enum struct Role {
    /// Buffer will be used as a uniform buffer
    UBO,

    /// Buffer will be used as a UBO with each element being bindable
    UBOBindSlots,

    /// Buffer will be used as a storage buffer
    SSBO
};

/**
 * @brief Helper struct that provides alignment and usage flags for buffer roles
 *
 * @tparam BufferRole The role to get alignment and usage flags for
 * @ingroup Renderer
 */
template<Role BufferRole>
struct RoleInfo {
    /**
     * @brief Returns the alignment to use for the given role
     */
    static constexpr Alignment getAlignment() {
        switch (BufferRole) {
        case Role::UBO:
            return Alignment::Std140;
        case Role::UBOBindSlots:
            return Alignment::UboBindOffset;
        case Role::SSBO:
            return Alignment::Std430;
        default:
            return Alignment::Packed;
        }
    }

    /**
     * @brief Returns the usage flags to use for the given role
     */
    static constexpr VkBufferUsageFlagBits getUsage() {
        switch (BufferRole) {
        case Role::UBO:
        case Role::UBOBindSlots:
            return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        case Role::SSBO:
            return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        default:
            return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
    }

    /**
     * @brief The alignment to use for the given role
     */
    static constexpr Alignment DataAlignment = getAlignment();

    /**
     * @brief The usage flags to use for the given role
     */
    static constexpr VkBufferUsageFlagBits Usage = getUsage();
};

} // namespace buf
} // namespace rc
} // namespace bl

#endif
