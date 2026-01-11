#include <BLIB/Render/Graph/Strategies/OverlayRenderStrategy.hpp>

#include <BLIB/Render/Graph/RenderGraph.hpp>
#include <BLIB/Render/Graph/Tasks/RenderOverlayTask.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
void OverlayRenderStrategy::populate(rg::RenderGraph& graph) { graph.putTask<RenderOverlayTask>(); }

} // namespace rgi
} // namespace rc
} // namespace bl
