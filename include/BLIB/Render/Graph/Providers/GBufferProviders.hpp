#ifndef BLIB_RENDER_GRAPH_PROVIDERS_GBUFFERPROVIDERS_HPP
#define BLIB_RENDER_GRAPH_PROVIDERS_GBUFFERPROVIDERS_HPP

#include <BLIB/Render/Graph/Assets/GBufferAssets.hpp>
#include <BLIB/Render/Graph/Providers/GenericTargetProvider.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief GBuffer provider for deferred rendering
 *
 * @ingroup Renderer
 */
using GBufferProvider =
    GenericTargetProvider<cfg::RenderPassIds::DeferredObjectPass, 4,
                          RenderPassBehavior::StartedByAsset,
                          DepthAttachmentType::SharedDepthBuffer, MSAABehavior::UseSettings>;

/**
 * @brief GBuffer provider for deferred HDR rendering
 *
 * @ingroup Renderer
 */
using GBufferHDRProvider =
    GenericTargetProvider<cfg::RenderPassIds::HDRDeferredObjectPass, 4,
                          RenderPassBehavior::StartedByAsset,
                          DepthAttachmentType::SharedDepthBuffer, MSAABehavior::UseSettings>;

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
