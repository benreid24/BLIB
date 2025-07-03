#ifndef BLIB_RENDER_GRAPH_ASSETS_BLOOMASSETS_HPP
#define BLIB_RENDER_GRAPH_ASSETS_BLOOMASSETS_HPP

#include <BLIB/Render/Config/RenderPassIds.hpp>
#include <BLIB/Render/Graph/Assets/GenericTargetAsset.hpp>
#include <BLIB/Render/Graph/MultiAsset.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Single color attachment asset for the bloom pass
 *
 * @ingroup Renderer
 */
using BloomColorAttachmentAsset =
    rgi::GenericTargetAsset<cfg::RenderPassIds::BloomPass, 1, RenderPassBehavior::StartedByTask,
                            DepthAttachmentType::None>;

/**
 * @brief Pair of BloomColorAttachment used for the bloom pass
 *
 * @ingroup Renderer
 */
using BloomColorAttachmentPairAsset = rg::MultiAsset<BloomColorAttachmentAsset, 2>;

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
