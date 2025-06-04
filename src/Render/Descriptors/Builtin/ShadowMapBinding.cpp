#include <BLIB/Render/Descriptors/Builtin/ShadowMapBinding.hpp>

#include <BLIB/Render/Scenes/ExtraContexts.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
ShadowMapBinding::ShadowMapBinding()
: Binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC) {}

void ShadowMapBinding::init(vk::VulkanState&, ShaderInputStore& inputStore) {
    storage = inputStore.getShaderInputWithId<ShadowMapCameraShaderInput>(ShadowMapCameraInputName);
    storage->getBuffer().transferEveryFrame();
}

void ShadowMapBinding::writeSet(SetWriteHelper& writer, VkDescriptorSet set, UpdateSpeed,
                                std::uint32_t) {
    VkDescriptorBufferInfo& bufferInfo = writer.getNewBufferInfo();
    bufferInfo.buffer                  = storage->getBuffer().gpuBufferHandle().getBuffer();
    bufferInfo.offset                  = 0;
    bufferInfo.range                   = sizeof(ShadowMapCameraPayload);

    VkWriteDescriptorSet& write = writer.getNewSetWrite(set);
    write.dstBinding            = getBindingIndex();
    write.pBufferInfo           = &bufferInfo;
    write.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
}

void ShadowMapBinding::onFrameStart() {
    // Scene3DInstance copies the light cameras into the buffer
}

std::uint32_t ShadowMapBinding::getDynamicOffsetForPipeline(scene::SceneRenderContext& ctx,
                                                            VkPipelineLayout, std::uint32_t,
                                                            UpdateSpeed) {
    scene::ctx::ShadowMapContext* shadowCtx = ctx.getExtraContext<scene::ctx::ShadowMapContext>();
    if (!shadowCtx) {
#ifdef BLIB_DEBUG
        static bool warned = false;
        if (!warned) {
            warned = true;
            BL_LOG_ERROR << "ShadowMapBinding requires a ShadowMapContext to be set in the "
                            "SceneRenderContext";
        }
#endif
        return 0;
    }
    switch (shadowCtx->lightType) {
    case scene::ctx::ShadowMapContext::SunLight:
        return 0;
    case scene::ctx::ShadowMapContext::SpotLight:
        return storage->getBuffer().alignedUniformSize() * (shadowCtx->lightIndex + 1);
    case scene::ctx::ShadowMapContext::PointLight:
        return storage->getBuffer().alignedUniformSize() *
               (shadowCtx->lightIndex + Config::MaxSpotShadows + 1);
    default:
        return 0;
    }
}

} // namespace ds
} // namespace rc
} // namespace bl
