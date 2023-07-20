#ifndef BLIB_RENDER_GRAPH_ASSETS_FINALSWAPFRAME_HPP
#define BLIB_RENDER_GRAPH_ASSETS_FINALSWAPFRAME_HPP

#include <BLIB/Render/Graph/Asset.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
class FinalSwapframe : public rg::Asset {
public:
    //

private:
    virtual void doCreate(engine::Engine& engine, Renderer& renderer) override;
    virtual void doPrepareForInput(const rg::ExecutionContext& context) override;
    virtual void doPrepareForOutput(const rg::ExecutionContext& context) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
