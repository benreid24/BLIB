#include <BLIB/Render/Graph/Assets/AutoExposureWorkBuffer.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
AutoExposureWorkBuffer::AutoExposureWorkBuffer()
: Asset(rg::AssetTags::AutoExposureWorkBuffer, false) {}

void AutoExposureWorkBuffer::doCreate(const rg::InitContext& ctx) {
    buffer = ctx.target.getShaderResources().getShaderResourceWithKey(
        sri::AutoExposureBufferShaderResourceKey);
}

void AutoExposureWorkBuffer::doPrepareForInput(const rg::ExecutionContext& ctx) {
    // ensure accumulate is done writing
    buffer->getBuffer().gpuBufferHandle().recordBarrier(ctx.commandBuffer,
                                                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                        VK_ACCESS_MEMORY_WRITE_BIT,
                                                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                        VK_ACCESS_MEMORY_READ_BIT);
}

void AutoExposureWorkBuffer::doStartOutput(const rg::ExecutionContext& ctx) {
    // ensure prior frame adjust is done reading
    buffer->getBuffer().gpuBufferHandle().recordBarrier(ctx.commandBuffer,
                                                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                        VK_ACCESS_MEMORY_READ_BIT,
                                                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                        VK_ACCESS_MEMORY_WRITE_BIT);

    // reset
    float zero = 0.f;
    vkCmdUpdateBuffer(ctx.commandBuffer,
                      buffer->getBuffer().gpuBufferHandle().getBuffer(),
                      0,
                      sizeof(float),
                      &zero);

    // ensure transfer is done before compute write
    buffer->getBuffer().gpuBufferHandle().recordBarrier(ctx.commandBuffer,
                                                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                        VK_ACCESS_MEMORY_WRITE_BIT,
                                                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                        VK_ACCESS_SHADER_WRITE_BIT);
}

} // namespace rgi
} // namespace rc
} // namespace bl
