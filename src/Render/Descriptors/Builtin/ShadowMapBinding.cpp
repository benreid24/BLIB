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
: Binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
, lighting(nullptr) {}

void ShadowMapBinding::setLighting(Scene3DInstance* l) { lighting = l; }

void ShadowMapBinding::init(vk::VulkanState&, DescriptorComponentStorageCache&) {}

void ShadowMapBinding::writeSet(SetWriteHelper& writer, VkDescriptorSet set, UpdateSpeed,
                                std::uint32_t frameIndex) {
    VkDescriptorBufferInfo& bufferInfo = writer.getNewBufferInfo();
    bufferInfo.buffer                  = lighting->getBufferBindingInfo(frameIndex).buffer;
    bufferInfo.offset                  = 0;
    bufferInfo.range                   = sizeof(Payload);

    VkWriteDescriptorSet& write = writer.getNewSetWrite(set);
    write.dstBinding            = getBindingIndex();
    write.pBufferInfo           = &bufferInfo;
    write.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
}

void ShadowMapBinding::onFrameStart() {}

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
        return lgt::LightingDescriptor3D::getSunlightCameraMatrixOffset();
    case scene::ctx::ShadowMapContext::SpotLight:
        return lighting->getUniform().getMatrixOffsetForSpotLight(shadowCtx->lightIndex);
    case scene::ctx::ShadowMapContext::PointLight:
        return lighting->getUniform().getMatrixOffsetForPointLight(shadowCtx->lightIndex);
    default:
        return 0;
    }
}

} // namespace ds
} // namespace rc
} // namespace bl
