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
namespace ds
{
Scene3DInstance::Scene3DInstance(Renderer& renderer, VkDescriptorSetLayout layout)
: SceneDescriptorSetInstance(renderer.vulkanState(), layout)
, renderer(renderer)
, shadowMaps(nullptr)
, ssaoBuffer(nullptr) {}

Scene3DInstance::~Scene3DInstance() {
    cleanup();
    uniform.destroy();
}

void Scene3DInstance::bind(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout,
                           std::uint32_t bindIndex, std::uint32_t observerIndex) {
    vkCmdBindDescriptorSets(commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout,
                            bindIndex,
                            1,
                            &descriptorSets.current(observerIndex),
                            0,
                            nullptr);
}

void Scene3DInstance::bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                      std::uint32_t setIndex, UpdateSpeed) const {
    vkCmdBindDescriptorSets(ctx.getCommandBuffer(),
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout,
                            setIndex,
                            1,
                            &descriptorSets.current(ctx.currentObserverIndex()),
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

void Scene3DInstance::init(ShaderInputStore& inputStore) {
    shadowMapCameras =
        inputStore.getShaderInputWithId<ShadowMapCameraShaderInput>(ShadowMapCameraInputName);

    // allocate memory
    createCameraBuffer();
    uniform.create(vulkanState, 1);

    emptySpotShadowMap.create(
        vulkanState,
        {.type       = vk::ImageOptions::Type::Image2D,
         .format     = vulkanState.findDepthFormat(),
         .usage      = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
         .extent     = renderer.getSettings().getShadowMapResolution(),
         .aspect     = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
         .viewAspect = VK_IMAGE_ASPECT_DEPTH_BIT});
    emptyPointShadowMap.create(
        vulkanState,
        {.type       = vk::ImageOptions::Type::Cubemap,
         .format     = vulkanState.findDepthFormat(),
         .usage      = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
         .extent     = renderer.getSettings().getShadowMapResolution(),
         .aspect     = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
         .viewAspect = VK_IMAGE_ASPECT_DEPTH_BIT});
    emptySSAOImage.create(vulkanState,
                          {.type   = vk::ImageOptions::Type::Image2D,
                           .format = vk::CommonTextureFormats::SingleChannelUnorm8,
                           .usage  = VK_IMAGE_USAGE_SAMPLED_BIT,
                           .extent = {4, 4},
                           .aspect = VK_IMAGE_ASPECT_COLOR_BIT});

    auto commandBuffer = vulkanState.sharedCommandPool.createBuffer();
    emptySpotShadowMap.clearAndTransition(
        commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, {.depthStencil = {1.f, 0}});
    emptyPointShadowMap.clearAndTransition(
        commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, {.depthStencil = {1.f, 0}});
    emptySSAOImage.clearAndTransition(
        commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, {.color = {1.f, 1.f, 1.f, 1.f}});
    commandBuffer.submit();

    // allocate descriptors
    allocateDescriptorSets();

    // create and configureWrite descriptors
    const std::uint32_t bufferWriteCount = cfg::Limits::MaxConcurrentFrames * 4;
    SetWriteHelper setWriter;
    setWriter.hintWriteCount(descriptorSets.size() * bufferWriteCount);
    setWriter.hintBufferInfoCount(descriptorSets.size() * bufferWriteCount);

    // write descriptors
    for (std::uint32_t i = 0; i < cfg::Limits::MaxSceneObservers; ++i) {
        for (std::uint32_t j = 0; j < cfg::Limits::MaxConcurrentFrames; ++j) {
            const auto set = descriptorSets.getRaw(i, j);

            writeCameraDescriptor(setWriter, i, j);

            // TODO - do we need padding for dynamic binding?
            VkDescriptorBufferInfo& lightInfoBufferInfo = setWriter.getNewBufferInfo();
            lightInfoBufferInfo.buffer                  = uniform.gpuBufferHandle().getBuffer();
            lightInfoBufferInfo.offset                  = 0;
            lightInfoBufferInfo.range                   = uniform.totalAlignedSize();

            VkWriteDescriptorSet& lightInfoWrite = setWriter.getNewSetWrite(set);
            lightInfoWrite.descriptorCount       = 1;
            lightInfoWrite.dstBinding            = 1;
            lightInfoWrite.dstArrayElement       = 0;
            lightInfoWrite.pBufferInfo           = &lightInfoBufferInfo;
            lightInfoWrite.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
    }
    setWriter.performWrite(vulkanState.device);
    uniform.transferEveryFrame();

    updateImageDescriptors();

    subscribe(renderer.getSignalChannel());
}

void Scene3DInstance::updateImageDescriptors() {
    const std::uint32_t imageWriteCount =
        cfg::Limits::MaxConcurrentFrames *
        (cfg::Limits::MaxSpotShadows + cfg::Limits::MaxPointShadows + 1); // +1 for ssao buffer
    SetWriteHelper setWriter;
    setWriter.hintWriteCount(descriptorSets.size() * imageWriteCount);
    setWriter.hintImageInfoCount(descriptorSets.size() * imageWriteCount);

    for (std::uint32_t i = 0; i < cfg::Limits::MaxSceneObservers; ++i) {
        for (std::uint32_t j = 0; j < cfg::Limits::MaxConcurrentFrames; ++j) {
            const auto set = descriptorSets.getRaw(i, j);

            VkSampler sampler = renderer.samplerCache().shadowMap();
            for (unsigned int k = 0; k < cfg::Limits::MaxSpotShadows; ++k) {
                VkDescriptorImageInfo& imageInfo = setWriter.getNewImageInfo();
                imageInfo.imageLayout            = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = shadowMaps ? shadowMaps->getSpotShadowImage(k).getView() :
                                                   emptySpotShadowMap.getView();
                imageInfo.sampler   = sampler;

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
            VkDescriptorImageInfo& ssaoInfo = setWriter.getNewImageInfo();
            ssaoInfo.imageLayout            = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            ssaoInfo.imageView = ssaoBuffer ? ssaoBuffer->getAttachmentSets()[0]->getImageView(0) :
                                              emptySSAOImage.getView();
            ssaoInfo.sampler   = renderer.samplerCache().noFilterEdgeClamped();

            VkWriteDescriptorSet& ssaoWrite = setWriter.getNewSetWrite(set);
            ssaoWrite.descriptorCount       = 1;
            ssaoWrite.dstBinding            = 4;
            ssaoWrite.dstArrayElement       = 0;
            ssaoWrite.pImageInfo            = &ssaoInfo;
            ssaoWrite.descriptorType        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        }
    }
    setWriter.performWrite(vulkanState.device);
}

void Scene3DInstance::process(const event::SceneGraphAssetInitialized& event) {
    if (event.scene == static_cast<Scene*>(owner)) {
        rgi::ShadowMapAsset* sm = dynamic_cast<rgi::ShadowMapAsset*>(&event.asset->asset.get());
        if (sm) {
            shadowMaps = sm;
            updateImageDescriptors();
        }
        else {
            if (event.asset->purpose == rgi::Purpose::SSAOBuffer) {
                rgi::SSAOAsset* ssao = dynamic_cast<rgi::SSAOAsset*>(&event.asset->asset.get());
                if (ssao) {
                    ssaoBuffer = ssao;
                    updateImageDescriptors();
                }
            }
        }
    }
}

void Scene3DInstance::process(const event::ShadowMapsInvalidated& e) {
    if (shadowMaps == e.asset) { updateImageDescriptors(); }
}

bool Scene3DInstance::allocateObject(ecs::Entity, scene::Key) {
    // n/a
    return true;
}

void Scene3DInstance::handleFrameStart() {
    auto& cameras = shadowMapCameras->getBuffer();
    auto& data    = getUniform();

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
}

} // namespace ds
} // namespace rc
} // namespace bl
