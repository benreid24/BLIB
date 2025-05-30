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

void ShadowMapBinding::setLighting(lgt::Scene3DLighting& l) { lighting = &l; }

void ShadowMapBinding::init(vk::VulkanState& vulkanState, DescriptorComponentStorageCache&) {
    cameraMatrices.create(vulkanState, Config::MaxSpotShadows + Config::MaxPointShadows * 6);
    cameraMatrices.transferEveryFrame();
}

void ShadowMapBinding::writeSet(SetWriteHelper& writer, VkDescriptorSet set, UpdateSpeed,
                                std::uint32_t frameIndex) {
    VkDescriptorBufferInfo& bufferInfo = writer.getNewBufferInfo();
    bufferInfo.buffer = cameraMatrices.gpuBufferHandles().getRaw(frameIndex).getBuffer();
    bufferInfo.offset = 0;
    bufferInfo.range  = cameraMatrices.alignedUniformSize() * 6;

    VkWriteDescriptorSet& write = writer.getNewSetWrite(set);
    write.dstBinding            = getBindingIndex();
    write.pBufferInfo           = &bufferInfo;
    write.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
}

void ShadowMapBinding::onFrameStart() {
    if (!lighting) { return; }

    // sunlight camera
    // TODO - set matrices dynamically from camera (or cascaded shadow maps)
    const lgt::SunLight3D& sun = lighting->getSun();
    Payload& sunlightCam       = cameraMatrices[0];
    sunlightCam.projection     = glm::ortho(-10.f, 10.f, -10.f, 10.f, 0.1f, 100.f);
    sunlightCam.view           = glm::lookAt(
        lighting->getSunPosition(), lighting->getSunPosition() + sun.dir, Config::UpDirection);

    // spot lights
    for (std::uint32_t i = 0; i < lighting->getSpotShadowCount(); ++i) {
        lgt::SpotLight3D& spot = lighting->getSpotlightUnsafe(i);
        cameraMatrices[i + 1].projection =
            glm::perspective(spot.outerCutoff * 2.f, 1.f, 0.1f, spot.computeFalloffRadius());
        cameraMatrices[i + 1].view =
            glm::lookAt(spot.pos, spot.pos + spot.dir, Config::UpDirection);
    }

    // point lights
    for (unsigned int i = 0; i < lighting->getPointLightCount(); ++i) {
        lgt::PointLight3D& point = lighting->getPointLightUnsafe(i);
        const glm::mat4 proj =
            glm::perspective(glm::radians(90.f), 1.f, 0.1f, point.computeFalloffRadius());
        for (unsigned int j = 0; j < 6; ++j) {
            Payload& pointCam   = cameraMatrices[(i + 1) * 6 + j];
            pointCam.projection = proj;
            pointCam.view       = glm::lookAt(
                point.pos, point.pos + Config::CubemapDirections[j], Config::UpDirection);
        }
    }
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
        return 0; // the sun is always light 0
    case scene::ctx::ShadowMapContext::SpotLight:
        return (shadowCtx->lightIndex + 1) * cameraMatrices.alignedUniformSize();
    case scene::ctx::ShadowMapContext::PointLight:
        return ((shadowCtx->lightIndex + Config::MaxSpotShadows + 1) * 6) *
               cameraMatrices.alignedUniformSize();
    default:
        return 0;
    }
}

} // namespace ds
} // namespace rc
} // namespace bl
