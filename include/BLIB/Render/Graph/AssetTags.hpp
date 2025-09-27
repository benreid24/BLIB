#ifndef BLIB_RENDER_GRAPH_ASSETTAGS_HPP
#define BLIB_RENDER_GRAPH_ASSETTAGS_HPP

#include <string_view>

namespace bl
{
namespace rc
{
namespace rg
{
/**
 * @brief Collection of built-in asset tags
 *
 * @ingroup Renderer
 */
struct AssetTags {
    static constexpr std::string_view GBuffer                  = "_builtin_GBuffer";
    static constexpr std::string_view RenderedSceneOutput      = "_builtin_RenderedSceneOutput";
    static constexpr std::string_view PostFXOutput             = "_builtin_PostFXOutput";
    static constexpr std::string_view FinalFrameOutput         = "_builtin_FinalFrameOutput";
    static constexpr std::string_view SceneInput               = "_builtin_SceneInput";
    static constexpr std::string_view OverlayInput             = "_builtin_OverlayInput";
    static constexpr std::string_view BloomColorAttachmentPair = "_builtin_BloomColorImagePair";
    static constexpr std::string_view ShadowMaps               = "_builtin_ShadowMaps";
    static constexpr std::string_view DepthBuffer              = "_builtin_DepthBuffer";
    static constexpr std::string_view SSAOBuffer               = "_builtin_SSAOBuffer";
    static constexpr std::string_view AutoExposureWorkBuffer   = "_builtin_AutoExposureWorkBuffer";
    static constexpr std::string_view ConsumedNextFrame        = "_builtin_ConsumedNextFrame";
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
