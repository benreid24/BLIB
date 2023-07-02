#include <BLIB/Render/Scenes/SceneRenderContext.hpp>

#include <BLIB/Render/Primitives/IndexBuffer.hpp>

namespace bl
{
namespace gfx
{
namespace scene
{
SceneRenderContext::SceneRenderContext(VkCommandBuffer commandBuffer, std::uint32_t observerIndex,
                                       const VkViewport& vp, std::uint32_t rpid, bool isrt)
: commandBuffer(commandBuffer)
, observerIndex(observerIndex)
, prevVB(nullptr)
, prevIB(nullptr)
, viewport(vp)
, renderPassId(rpid)
, isRenderTexture(isrt) {
    boundDescriptors.fill(nullptr);
}

void SceneRenderContext::bindPipeline(vk::Pipeline& pipeline) {
    pipeline.bind(commandBuffer, renderPassId);
}

void SceneRenderContext::bindDescriptors(VkPipelineLayout layout,
                                         ds::DescriptorSetInstance** descriptors,
                                         std::uint32_t descriptorCount) {
    bool bind = false;
    for (unsigned int i = 0; i < descriptorCount; ++i) {
        if (bind || descriptors[i] != boundDescriptors[i]) {
            bind                = true;
            boundDescriptors[i] = descriptors[i];
            descriptors[i]->bindForPipeline(*this, layout, i);
        }
    }
}

void SceneRenderContext::renderObject(const SceneObject& object) {
    if (prevVB != object.drawParams.vertexBuffer) {
        prevVB = object.drawParams.vertexBuffer;

        VkBuffer vertexBuffers[] = {object.drawParams.vertexBuffer};
        VkDeviceSize offsets[]   = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    }
    if (prevIB != object.drawParams.indexBuffer) {
        prevIB = object.drawParams.indexBuffer;
        vkCmdBindIndexBuffer(
            commandBuffer, object.drawParams.indexBuffer, 0, prim::IndexBuffer::IndexType);
    }

    vkCmdDrawIndexed(commandBuffer,
                     object.drawParams.indexCount,
                     object.drawParams.instanceCount,
                     object.drawParams.indexOffset,
                     object.drawParams.vertexOffset,
                     object.drawParams.instanceCount == 1 ? object.sceneId :
                                                            object.drawParams.firstInstance);
}

} // namespace scene
} // namespace gfx
} // namespace bl
