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
, perObjStart(0)
, perObjCount(0)
, viewport(vp)
, renderPassId(rpid)
, isRenderTexture(isrt) {
    boundDescriptors.fill(nullptr);
    perObjDescriptors.fill(nullptr);
}

void SceneRenderContext::bindPipeline(vk::Pipeline& pipeline) {
    pipeline.bind(commandBuffer, renderPassId);
}

void SceneRenderContext::bindDescriptors(VkPipelineLayout layout,
                                         ds::DescriptorSetInstance** descriptors,
                                         std::uint32_t descriptorCount) {
    bool bind   = false;
    perObjCount = 0;
    for (unsigned int i = 0; i < descriptorCount; ++i) {
        if (bind || descriptors[i] != boundDescriptors[i]) {
            bind                = true;
            boundDescriptors[i] = descriptors[i];
            descriptors[i]->bindForPipeline(*this, layout, i);
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
        perObjDescriptors[i - perObjStart]->bindForObject(*this, layout, i, object.sceneId);
    }

    vkCmdDrawIndexed(commandBuffer,
                     object.drawParams.indexCount,
                     object.drawParams.instanceCount,
                     object.drawParams.indexOffset,
                     object.drawParams.vertexOffset,
                     object.drawParams.firstInstance);
}

} // namespace scene
} // namespace gfx
} // namespace bl
