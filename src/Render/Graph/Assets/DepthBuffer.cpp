#include <BLIB/Render/Graph/Assets/DepthBuffer.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/RenderTarget.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
DepthBuffer::DepthBuffer()
: Asset(rg::AssetTags::DepthBuffer, false)
, buffer(nullptr)
, cleared(false) {}

void DepthBuffer::doCreate(const rg::InitContext& ctx) {
    buffer = ctx.getShaderResourceStore(sr::StoreKey::Observer)
                 .getShaderResourceWithKey(sri::DepthBufferResourceKey);
}

void DepthBuffer::onResize(glm::u32vec2) {
    // noop
}

void DepthBuffer::setSizeMode(sri::DepthBufferShaderResource::SizeMode m) {
    buffer->setSizeMode(m);
}

void DepthBuffer::clear(VkCommandBuffer commandBuffer) { buffer->clear(commandBuffer); }

void DepthBuffer::onReset() { cleared = false; }

void DepthBuffer::doStartOutput(const rg::ExecutionContext& ctx) {
    if (!cleared) {
        cleared = true;
        clear(ctx.commandBuffer);
    }
}

void DepthBuffer::ensureValid(const glm::u32vec2& size, VkSampleCountFlagBits samples) {
    if (buffer) { buffer->ensureValid(size, samples); }
}

} // namespace rgi
} // namespace rc
} // namespace bl
