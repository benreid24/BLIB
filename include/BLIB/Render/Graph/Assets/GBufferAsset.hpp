#ifndef BLIB_RENDER_GRAPH_ASSETS_GBUFFERASSETS_HPP
#define BLIB_RENDER_GRAPH_ASSETS_GBUFFERASSETS_HPP

#include <BLIB/Render/Config/RenderPassIds.hpp>
#include <BLIB/Render/Graph/Assets/GenericTargetAsset.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief GBuffer asset for deferred rendering
 *
 * @ingroup Renderer
 */
using GBufferAsset =
    GenericTargetAsset<cfg::RenderPassIds::DeferredObjectPass, 4,
                       RenderPassBehavior::StartedByAsset, DepthAttachmentType::SharedDepthBuffer,
                       MSAABehavior::UseSettings>;

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
