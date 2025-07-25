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
    static constexpr std::uint32_t StandardAttachmentPass = 0;
    static constexpr std::uint32_t SwapchainPass          = 1;
    static constexpr std::uint32_t HDRAttachmentPass      = 2;
    static constexpr std::uint32_t BloomPass              = 3;
    static constexpr std::uint32_t SSAOPass               = 4;
    static constexpr std::uint32_t ShadowMapPass          = 5;
    static constexpr std::uint32_t DeferredObjectPass     = 6;
    static constexpr std::uint32_t HDRDeferredObjectPass  = 7;
};

} // namespace cfg
} // namespace rc
} // namespace bl

#endif
