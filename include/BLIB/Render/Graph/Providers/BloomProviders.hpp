#ifndef BLIB_RENDER_GRAPH_PROVIDERS_BLOOMPROVIDERS_HPP
#define BLIB_RENDER_GRAPH_PROVIDERS_BLOOMPROVIDERS_HPP

#include <BLIB/Render/Graph/Assets/BloomAssets.hpp>
#include <BLIB/Render/Graph/Providers/GenericTargetProvider.hpp>
#include <BLIB/Render/Graph/Providers/MultiAssetProvider.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Provider for a single bloom color attachment asset
 *
 * @ingroup Renderer
 */
using BloomColorAttachmentProvider =
    GenericTargetProvider<Config::RenderPassIds::BloomPass, 1, RenderPassBehavior::StartedByTask>;

/**
 * @brief Provider for a pair of bloom color attachment assets
 *
 * @ingroup Renderer
 */
using BloomColorAttachmentPairProvider =
    MultiAssetProvider<BloomColorAttachmentProvider, BloomColorAttachmentAsset, 2>;

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
