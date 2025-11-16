#include <BLIB/Render/Descriptors/Builtin/Scene3DInstance.hpp>

#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Descriptors/SetWriteHelper.hpp>
#include <BLIB/Render/Graph/Assets/ShadowMapAsset.hpp>
#include <BLIB/Render/Graph/Purpose.hpp>
#include <BLIB/Render/Lighting/Scene3DLighting.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Scenes/Scene3D.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <array>

namespace bl
{
namespace rc
{
namespace dsi
{
Scene3DInstance::Scene3DInstance(Renderer& renderer, VkDescriptorSetLayout layout)
: DescriptorSetInstance(Bindless, SpeedAgnostic)
, renderer(renderer)
, setLayout(layout)
, shadowMaps(nullptr)
, ssaoBuffer(nullptr)
, deferredImageUpdates(0) {}

Scene3DInstance::~Scene3DInstance() {
    renderer.vulkanState().descriptorPool.release(allocHandle, descriptorSets.rawData());
}

void Scene3DInstance::bind(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint,
                           VkPipelineLayout pipelineLayout, std::uint32_t bindIndex) {
    vkCmdBindDescriptorSets(commandBuffer,
                            bindPoint,
                            pipelineLayout,
                            bindIndex,
                            1,
                            &descriptorSets.current(),
                            0,
                            nullptr);
}

void Scene3DInstance::bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                      std::uint32_t setIndex, UpdateSpeed) const {
    vkCmdBindDescriptorSets(ctx.getCommandBuffer(),
                            ctx.getPipelineBindPoint(),
                            layout,
                            setIndex,
                            1,
                            &descriptorSets.current(),
                            0,
                            nullptr);
}

void Scene3DInstance::bindForObject(scene::SceneRenderContext&, VkPipelineLayout, std::uint32_t,
                                    scene::Key) const {
    // n/a
}

void Scene3DInstance::releaseObject(ecs::Entity, scene::Key) {
    // n/a
}

void Scene3DInstance::init(ds::InitContext& ctx) {
    cameraBuffer = ctx.observerShaderResources.getShaderResourceWithKey(sri::CameraBufferKey);
    lightBuffer  = ctx.sceneShaderResources.getShaderResourceWithKey(sri::Scene3DLightingKey);
    shadowMapCameras =
        ctx.sceneShaderResources.getShaderResourceWithKey(sri::ShadowMapCameraShaderResourceKey);
    shadowMaps = ctx.sceneShaderResources.getShaderResourceWithKey(sri::ShadowMapResourceKey);
    ssaoBuffer = ctx.observerShaderResources.getShaderResourceWithKey(
        rgi::makeShaderResourceKey<rgi::SSAOShaderResource>(
            rg::AssetTags::SSAOBuffer, rgi::Purpose::SSAOBuffer, 0));

    emptySpotShadowMap.create(
        renderer.vulkanState(),
        {.type       = vk::ImageOptions::Type::Image2D,
         .format     = renderer.vulkanState().findDepthFormat(),
         .usage      = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
         .extent     = renderer.getSettings().getShadowMapResolution(),
         .aspect     = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
         .viewAspect = VK_IMAGE_ASPECT_DEPTH_BIT});
    emptyPointShadowMap.create(
        renderer.vulkanState(),
        {.type       = vk::ImageOptions::Type::Cubemap,
         .format     = renderer.vulkanState().findDepthFormat(),
         .usage      = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
         .extent     = renderer.getSettings().getShadowMapResolution(),
         .aspect     = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
         .viewAspect = VK_IMAGE_ASPECT_DEPTH_BIT});
    emptySSAOImage.create(renderer.vulkanState(),
                          {.type   = vk::ImageOptions::Type::Image2D,
                           .format = vk::CommonTextureFormats::SingleChannelUnorm8,
                           .usage  = VK_IMAGE_USAGE_SAMPLED_BIT,
                           .extent = {4, 4},
                           .aspect = VK_IMAGE_ASPECT_COLOR_BIT});

    auto commandBuffer = renderer.vulkanState().sharedCommandPool.createBuffer();
    emptySpotShadowMap.clearAndTransition(
        commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, {.depthStencil = {1.f, 0}});
    emptyPointShadowMap.clearAndTransition(
        commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, {.depthStencil = {1.f, 0}});
    emptySSAOImage.clearAndTransition(
        commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, {.color = {1.f, 1.f, 1.f, 1.f}});
    commandBuffer.submit();

    // allocate descriptors
    descriptorSets.emptyInit(renderer.vulkanState());
    allocHandle = renderer.vulkanState().descriptorPool.allocate(
        setLayout, descriptorSets.rawData(), descriptorSets.size());

    // create and configureWrite descriptors
    const std::uint32_t bufferWriteCount = cfg::Limits::MaxConcurrentFrames * 2;
    ds::SetWriteHelper setWriter;
    setWriter.hintWriteCount(bufferWriteCount);
    setWriter.hintBufferInfoCount(bufferWriteCount);

    // write descriptors
    for (std::uint32_t j = 0; j < cfg::Limits::MaxConcurrentFrames; ++j) {
        const auto set = descriptorSets.getRaw(j);

        VkDescriptorBufferInfo& cameraBufferInfo = setWriter.getNewBufferInfo();
        cameraBufferInfo.buffer                  = cameraBuffer->getBuffer().getRawBuffer(j);
        cameraBufferInfo.offset                  = 0;
        cameraBufferInfo.range                   = cameraBuffer->getBuffer().getTotalAlignedSize();

        VkWriteDescriptorSet& cameraWrite = setWriter.getNewSetWrite(set);
        cameraWrite.descriptorCount       = 1;
        cameraWrite.dstBinding            = 0;
        cameraWrite.dstArrayElement       = 0;
        cameraWrite.pBufferInfo           = &cameraBufferInfo;
        cameraWrite.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        VkDescriptorBufferInfo& lightInfoBufferInfo = setWriter.getNewBufferInfo();
        lightInfoBufferInfo.buffer = lightBuffer->getBuffer().getCurrentFrameRawBuffer();
        lightInfoBufferInfo.offset = 0;
        lightInfoBufferInfo.range  = lightBuffer->getBuffer().getTotalAlignedSize();

        VkWriteDescriptorSet& lightInfoWrite = setWriter.getNewSetWrite(set);
        lightInfoWrite.descriptorCount       = 1;
        lightInfoWrite.dstBinding            = 1;
        lightInfoWrite.dstArrayElement       = 0;
        lightInfoWrite.pBufferInfo           = &lightInfoBufferInfo;
        lightInfoWrite.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }

    setWriter.performWrite(renderer.vulkanState().device);

    cameraBuffer->getBuffer().transferEveryFrame();
    cameraBuffer->getBuffer().setCopyFullRange(true);
    lightBuffer->getBuffer().transferEveryFrame();
    lightBuffer->getBuffer().setCopyFullRange(true);

    updateImageDescriptors();

    subscribe(renderer.getSignalChannel());
}

void Scene3DInstance::updateImageDescriptors() {
    deferredImageUpdates = 0;
    for (unsigned int i = 0; i < cfg::Limits::MaxConcurrentFrames; ++i) {
        updateImageDescriptors(i);
    }
}

void Scene3DInstance::updateImageDescriptors(unsigned int frameIndex) {
    const std::uint32_t imageWriteCount =
        (cfg::Limits::MaxSpotShadows + cfg::Limits::MaxPointShadows + 1); // +1 for ssao buffer
    ds::SetWriteHelper setWriter;
    setWriter.hintWriteCount(imageWriteCount);
    setWriter.hintImageInfoCount(imageWriteCount);

    const auto set = descriptorSets.getRaw(frameIndex);

    VkSampler sampler = renderer.samplerCache().shadowMap();
    for (unsigned int k = 0; k < cfg::Limits::MaxSpotShadows; ++k) {
        VkDescriptorImageInfo& imageInfo = setWriter.getNewImageInfo();
        imageInfo.imageLayout            = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView =
            shadowMaps ? shadowMaps->getSpotShadowImage(k).getView() : emptySpotShadowMap.getView();
        imageInfo.sampler = sampler;

        VkWriteDescriptorSet& write = setWriter.getNewSetWrite(set);
        write.descriptorCount       = 1;
        write.dstBinding            = 2;
        write.dstArrayElement       = k;
        write.pImageInfo            = &imageInfo;
        write.descriptorType        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }

    for (unsigned int k = 0; k < cfg::Limits::MaxPointShadows; ++k) {
        VkDescriptorImageInfo& imageInfo = setWriter.getNewImageInfo();
        imageInfo.imageLayout            = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = shadowMaps ? shadowMaps->getPointShadowImage(k).getView() :
                                           emptyPointShadowMap.getView();
        imageInfo.sampler   = sampler;

        VkWriteDescriptorSet& write = setWriter.getNewSetWrite(set);
        write.descriptorCount       = 1;
        write.dstBinding            = 3;
        write.dstArrayElement       = k;
        write.pImageInfo            = &imageInfo;
        write.descriptorType        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }

    // ssao buffer
    const bool useSSAO = ssaoBuffer && renderer.getSettings().getSSAO() != Settings::SSAO::None;
    VkDescriptorImageInfo& ssaoInfo = setWriter.getNewImageInfo();
    ssaoInfo.imageLayout            = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    ssaoInfo.imageView = useSSAO ? ssaoBuffer->getImages().attachmentSet().getImageView(0) :
                                   emptySSAOImage.getView();
    ssaoInfo.sampler   = renderer.samplerCache().noFilterEdgeClamped();

    VkWriteDescriptorSet& ssaoWrite = setWriter.getNewSetWrite(set);
    ssaoWrite.descriptorCount       = 1;
    ssaoWrite.dstBinding            = 4;
    ssaoWrite.dstArrayElement       = 0;
    ssaoWrite.pImageInfo            = &ssaoInfo;
    ssaoWrite.descriptorType        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    setWriter.performWrite(renderer.vulkanState().device);
}

void Scene3DInstance::process(const event::ShadowMapsInvalidated& e) {
    if (shadowMaps == e.maps) { deferredImageUpdates = 1 << cfg::Limits::MaxConcurrentFrames; }
}

void Scene3DInstance::process(const event::SettingsChanged& e) {
    if (e.setting == event::SettingsChanged::SSAO) {
        deferredImageUpdates = 1 << cfg::Limits::MaxConcurrentFrames;
    }
}

bool Scene3DInstance::allocateObject(ecs::Entity, scene::Key) {
    // n/a
    return true;
}

void Scene3DInstance::updateDescriptors() {
    auto& cameras = shadowMapCameras->getBuffer();
    auto& data    = lightBuffer->getBuffer()[0];

    cameras[0].viewProj[0] = data.sun.viewProjectionMatrix;

    for (unsigned int i = 0; i < data.nSpotShadows; ++i) {
        cameras[i + 1].viewProj[0] = data.spotlightsWithShadows[i].getViewProjectionMatrix();
    }

    for (unsigned int i = 0; i < data.nPointShadows; ++i) {
        auto& light        = data.pointLightsWithShadows[i];
        auto& cam          = cameras[i + cfg::Limits::MaxSpotShadows];
        cam.posAndFarPlane = glm::vec4(light.pos, light.planes.farPlane);
        for (unsigned int j = 0; j < 6; ++j) { cam.viewProj[j] = light.viewProjectionMatrices[j]; }
    }

    if (deferredImageUpdates != 0) {
        deferredImageUpdates = deferredImageUpdates << 1;
        updateImageDescriptors(renderer.vulkanState().currentFrameIndex());
    }
}

} // namespace dsi
} // namespace rc
} // namespace bl
