#ifndef BLIB_RENDER_GRAPH_ASSETS_STANDARDATTACHMENTPROVIDER_HPP
#define BLIB_RENDER_GRAPH_ASSETS_STANDARDATTACHMENTPROVIDER_HPP

#include <BLIB/Render/Graph/Providers/GenericTargetProvider.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Helper typedef for standard target providers
 *
 * @ingroup Renderer
 */
using StandardTargetProvider =
    GenericTargetProvider<cfg::RenderPassIds::StandardAttachmentPass, 1,
                          RenderPassBehavior::StartedByAsset,
                          DepthAttachmentType::SharedDepthBuffer,
                          MSAABehavior::UseSettings | MSAABehavior::ResolveAttachments>;
} // namespace rgi
} // namespace rc
} // namespace bl

#endif
