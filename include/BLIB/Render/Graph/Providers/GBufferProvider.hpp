#ifndef BLIB_RENDER_GRAPH_PROVIDERS_GBUFFERPROVIDERS_HPP
#define BLIB_RENDER_GRAPH_PROVIDERS_GBUFFERPROVIDERS_HPP

#include <BLIB/Render/Graph/Assets/GBufferAsset.hpp>
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
using GBufferProvider = GenericTargetProvider<GBufferAsset>;

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
