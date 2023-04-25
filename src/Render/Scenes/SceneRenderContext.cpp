#include <BLIB/Render/Scenes/SceneRenderContext.hpp>

#include <BLIB/Render/Primitives/IndexBuffer.hpp>

namespace bl
{
namespace render
{
SceneRenderContext::SceneRenderContext(VkCommandBuffer commandBuffer, std::uint32_t observerIndex)
: commandBuffer(commandBuffer)
, observerIndex(observerIndex)
, prevVB(nullptr)
, prevIB(nullptr)
, perObjStart(0)
, perObjCount(0) {
    boundDescriptors.fill(nullptr);
    perObjDescriptors.fill(nullptr);
}

void SceneRenderContext::bindPipeline(VkPipeline pipeline) {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void SceneRenderContext::bindDescriptors(
    VkPipelineLayout layout, const std::vector<ds::DescriptorSetInstance*>& descriptors) {
    bool bind   = false;
    perObjCount = 0;
    for (unsigned int i = 0; i < descriptors.size(); ++i) {
        if (bind || descriptors[i] != boundDescriptors[i]) {
            bind                = true;
            boundDescriptors[i] = descriptors[i];
            descriptors[i]->bindForPipeline(commandBuffer, layout, observerIndex, i);
        }

        if (descriptors[i]->isPerObject()) {
            perObjDescriptors[perObjCount] = descriptors[i];
            if (perObjCount == 0) {
                perObjStart = i;
                perObjCount = 1;
            }
            else { ++perObjCount; }
        }
#ifdef BLIB_DEBUG
        else if (perObjCount > 0) {
            BL_LOG_CRITICAL
                << "Per-object descriptors must be contiguous in layout and be at the end";
            throw std::runtime_error(
                "Per-object descriptors must be contiguous in layout and be at the end");
        }
#endif
    }
}

void SceneRenderContext::renderObject(VkPipelineLayout layout, const SceneObject& object) {
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

    for (unsigned int i = perObjStart; i < perObjStart + perObjCount; ++i) {
        perObjDescriptors[i]->bindForObject(commandBuffer, layout, i, object.sceneId);
    }

    vkCmdDrawIndexed(commandBuffer,
                     object.drawParams.indexCount,
                     object.drawParams.instanceCount,
                     object.drawParams.indexOffset,
                     object.drawParams.vertexOffset,
                     object.drawParams.firstInstance);
}

} // namespace render
} // namespace bl
