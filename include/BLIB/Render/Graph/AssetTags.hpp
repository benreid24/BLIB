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
    static constexpr std::string_view RenderedSceneOutput    = "_builtin_RenderedSceneOutput";
    static constexpr std::string_view RenderedSceneOutputHDR = "_builtin_RenderedSceneOutputHDR";
    static constexpr std::string_view PostFXOutput           = "_builtin_PostFXOutput";
    static constexpr std::string_view FinalFrameOutput       = "_builtin_FinalFrameOutput";
    static constexpr std::string_view SceneObjectsInput      = "_builtin_SceneObjectsInput";
    static constexpr std::string_view ColorAttachmentSingle  = "_builtin_ColorAttachmentSingle";
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
