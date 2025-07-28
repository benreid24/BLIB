#ifndef BLIB_RENDER_GRAPH_TASKIDS_HPP
#define BLIB_RENDER_GRAPH_TASKIDS_HPP

#include <string_view>

namespace bl
{
namespace rc
{
namespace rg
{
struct TaskIds {
    static constexpr std::string_view ForwardRenderOpaqueTask = "__builtin_ForwardRenderOpaqueTask";
    static constexpr std::string_view ForwardRenderTransparentTask =
        "__builtin_ForwardRenderTransparentTask";
    static constexpr std::string_view DeferredRenderTask    = "__builtin_DeferredRenderTask";
    static constexpr std::string_view DeferredCompositeTask = "__builtin_DeferredCompositeTask";
    static constexpr std::string_view PostProcess3DTask     = "__builtin_PostProcess3DTask";
    static constexpr std::string_view BloomTask             = "__builtin_BloomTask";
    static constexpr std::string_view SSAOTask              = "__builtin_SSAOTask";
    static constexpr std::string_view ShadowMapTask         = "__builtin_ShadowMapTask";
    static constexpr std::string_view Outline3DTask         = "__builtin_Outline3DTask";
    static constexpr std::string_view FadeEffectTask        = "__builtin_FadeEffectTask";
    static constexpr std::string_view RenderOverlayTask     = "__builtin_RenderOverlayTask";
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
