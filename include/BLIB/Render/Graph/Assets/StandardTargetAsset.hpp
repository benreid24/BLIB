#ifndef BLIB_RENDER_GRAPH_ASSETS_STANDARDATTACHMENTASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_STANDARDATTACHMENTASSET_HPP

#include <BLIB/Render/Graph/Assets/GenericTargetAsset.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Helper typedef for standard target assets
 *
 * @ingroup Renderer
 */
using StandardTargetAsset =
    GenericTargetAsset<cfg::RenderPassIds::StandardAttachmentPass, 1,
                       RenderPassBehavior::StartedByAsset, DepthAttachmentType::SharedDepthBuffer,
                       MSAABehavior::UseSettings | MSAABehavior::ResolveAttachments>;

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
