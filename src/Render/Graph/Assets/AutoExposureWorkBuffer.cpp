#include <BLIB/Render/Graph/Assets/AutoExposureWorkBuffer.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
void AutoExposureWorkBuffer::doCreate(engine::Engine&, Renderer& renderer, RenderTarget*) {
    if (!buffer.create(renderer.vulkanState(),
                       sizeof(float) * 4,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                       VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                       0)) {
        throw std::runtime_error("Failed to create auto exposure work buffer");
    }
}

void AutoExposureWorkBuffer::doPrepareForInput(const rg::ExecutionContext& ctx) {
    // ensure accumulate is done writing
    buffer.recordBarrier(ctx.commandBuffer,
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                         VK_ACCESS_MEMORY_WRITE_BIT,
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                         VK_ACCESS_MEMORY_READ_BIT);
}

void AutoExposureWorkBuffer::doStartOutput(const rg::ExecutionContext& ctx) {
    // ensure prior frame adjust is done reading
    buffer.recordBarrier(ctx.commandBuffer,
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                         VK_ACCESS_MEMORY_READ_BIT,
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                         VK_ACCESS_MEMORY_WRITE_BIT);
}

} // namespace rgi
} // namespace rc
} // namespace bl
