#ifndef BLIB_RENDER_CONFIG_RENDERPASSIDS_HPP
#define BLIB_RENDER_CONFIG_RENDERPASSIDS_HPP

#include <cstdint>

namespace bl
{
namespace rc
{
namespace cfg
{
/**
 * @brief Built-in render passes
 *
 * @ingroup Renderer
 */
struct RenderPassIds {
    static constexpr std::uint32_t StandardAttachmentDefault = 0;
    static constexpr std::uint32_t SwapchainDefault          = 1;
    static constexpr std::uint32_t HDRAttachmentDefault      = 2;
    static constexpr std::uint32_t BloomPass                 = 3;
    static constexpr std::uint32_t ShadowMapPass             = 4;
};

} // namespace cfg
} // namespace rc
} // namespace bl

#endif
