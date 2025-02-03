#include <BLIB/Render/Scenes/SceneRenderContext.hpp>

#include <BLIB/Render/Buffers/IndexBuffer.hpp>

namespace bl
{
namespace rc
{
namespace scene
{
SceneRenderContext::SceneRenderContext(VkCommandBuffer commandBuffer, std::uint32_t observerIndex,
                                       const VkViewport& vp, RenderPhase phase, std::uint32_t rpid,
                                       bool isrt)
: commandBuffer(commandBuffer)
, observerIndex(observerIndex)
, renderPhase(phase)
, prevVB(nullptr)
, prevIB(nullptr)
, boundSpeed(std::numeric_limits<UpdateSpeed>::max())
, viewport(vp)
, renderPassId(rpid)
, isRenderTexture(isrt) {
    boundDescriptors.fill(nullptr);
}

void SceneRenderContext::bindPipeline(mat::MaterialPipeline& pipeline) {
    pipeline.bind(commandBuffer, renderPhase, renderPassId);
}

void SceneRenderContext::bindDescriptors(VkPipelineLayout layout, UpdateSpeed speed,
                                         ds::DescriptorSetInstance** descriptors,
                                         std::uint32_t descriptorCount) {
    const bool speedChange = speed != boundSpeed;
    bool bind              = false;
    for (unsigned int i = 0; i < descriptorCount; ++i) {
        if (bind || descriptors[i] != boundDescriptors[i] ||
            (descriptors[i]->needsRebindForNewSpeed() && speedChange)) {
            bind                = true;
            boundDescriptors[i] = descriptors[i];
            descriptors[i]->bindForPipeline(*this, layout, i, speed);
        }
    }
    boundSpeed = speed;
}

void SceneRenderContext::renderObject(const SceneObject& object) {
    if (prevVB != object.drawParams.vertexBuffer) {
        prevVB = object.drawParams.vertexBuffer;

        VkBuffer vertexBuffers[] = {object.drawParams.vertexBuffer};
        VkDeviceSize offsets[]   = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    }

    switch (object.drawParams.type) {
    case prim::DrawParameters::DrawType::VertexBuffer:
        vkCmdDraw(commandBuffer,
                  object.drawParams.vertexCount,
                  object.drawParams.instanceCount,
                  object.drawParams.vertexOffset,
                  object.drawParams.instanceCount == 1 ? object.sceneKey.sceneId :
                                                         object.drawParams.firstInstance);
        break;

    case prim::DrawParameters::DrawType::IndexBuffer:
        if (prevIB != object.drawParams.indexBuffer) {
            prevIB = object.drawParams.indexBuffer;
            vkCmdBindIndexBuffer(
                commandBuffer, object.drawParams.indexBuffer, 0, buf::IndexBuffer::IndexType);
        }
        vkCmdDrawIndexed(commandBuffer,
                         object.drawParams.indexCount,
                         object.drawParams.instanceCount,
                         object.drawParams.indexOffset,
                         object.drawParams.vertexOffset,
                         object.drawParams.instanceCount == 1 ? object.sceneKey.sceneId :
                                                                object.drawParams.firstInstance);
        break;
    }
}

} // namespace scene
} // namespace rc
} // namespace bl
