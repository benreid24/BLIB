#include <BLIB/Render/Scenes/SceneRenderContext.hpp>

#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>

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
, isRenderTexture(isrt)
, extraContextType(typeid(void))
, extraContext(nullptr) {
    boundDescriptors.fill(nullptr);
}

void SceneRenderContext::bindPipeline(mat::MaterialPipeline& pipeline, std::uint32_t spec) {
    pipeline.bind(commandBuffer, renderPhase, renderPassId, spec);
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
    const auto& drawParams = object.component->getDrawParameters();
    if (prevVB != drawParams.vertexBuffer) {
        prevVB = drawParams.vertexBuffer;

        VkBuffer vertexBuffers[] = {drawParams.vertexBuffer};
        VkDeviceSize offsets[]   = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    }

    switch (drawParams.type) {
    case prim::DrawParameters::DrawType::VertexBuffer:
        vkCmdDraw(commandBuffer,
                  drawParams.vertexCount,
                  drawParams.instanceCount,
                  drawParams.vertexOffset,
                  drawParams.instanceCount == 1 ? object.sceneKey.sceneId :
                                                  drawParams.firstInstance);
        break;

    case prim::DrawParameters::DrawType::IndexBuffer:
        if (prevIB != drawParams.indexBuffer) {
            prevIB = drawParams.indexBuffer;
            vkCmdBindIndexBuffer(
                commandBuffer, drawParams.indexBuffer, 0, buf::IndexBuffer::IndexType);
        }
        vkCmdDrawIndexed(commandBuffer,
                         drawParams.indexCount,
                         drawParams.instanceCount,
                         drawParams.indexOffset,
                         drawParams.vertexOffset,
                         drawParams.instanceCount == 1 ? object.sceneKey.sceneId :
                                                         drawParams.firstInstance);
        break;
    }
}

void SceneRenderContext::renderObject(const rcom::DrawableBase& object) {
    renderObject(*object.getSceneRef().object);
}

} // namespace scene
} // namespace rc
} // namespace bl
