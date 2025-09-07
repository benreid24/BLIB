#ifndef BLIB_RENDER_GRAPH_ASSETS_AUTOEXPOSUREWORKBUFFER_HPP
#define BLIB_RENDER_GRAPH_ASSETS_AUTOEXPOSUREWORKBUFFER_HPP

#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/GenericBufferAsset.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Asset that provides the work buffer for the auto exposure compute pipelines
 *
 * @ingroup Renderer
 */
class AutoExposureWorkBuffer : public rgi::GenericBufferAsset {
public:
    /**
     * @brief Creates the work buffer asset
     */
    AutoExposureWorkBuffer()
    : GenericBufferAsset(rg::AssetTags::AutoExposureWorkBuffer) {}

    /**
     * @brief Destroys the asset
     */
    virtual ~AutoExposureWorkBuffer() = default;

private:
    virtual void doCreate(engine::Engine&, Renderer&, RenderTarget*) override;
    virtual void doPrepareForInput(const rg::ExecutionContext&) override;
    virtual void doStartOutput(const rg::ExecutionContext&) override;
    virtual void doEndOutput(const rg::ExecutionContext&) override {}
    virtual void onResize(glm::u32vec2) override {}
    virtual void onReset() override {}
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
