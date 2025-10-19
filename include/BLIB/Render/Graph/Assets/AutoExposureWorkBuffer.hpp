#ifndef BLIB_RENDER_GRAPH_ASSETS_AUTOEXPOSUREWORKBUFFER_HPP
#define BLIB_RENDER_GRAPH_ASSETS_AUTOEXPOSUREWORKBUFFER_HPP

#include <BLIB/Render/Graph/Asset.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/ShaderResources/AutoExposureBufferShaderResource.hpp>

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
class AutoExposureWorkBuffer : public rg::Asset {
public:
    /**
     * @brief Creates the work buffer asset
     */
    AutoExposureWorkBuffer();

    /**
     * @brief Destroys the asset
     */
    virtual ~AutoExposureWorkBuffer() = default;

private:
    sri::AutoExposureBufferShaderResource* buffer;

    virtual void doCreate(const rg::InitContext& ctx) override;
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
