#include <BLIB/Render/Descriptors/Builtin/Scene3DInstance.hpp>

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/SetWriteHelper.hpp>
#include <BLIB/Render/Graph/Assets/ShadowMapAsset.hpp>
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
, renderer(renderer) {}

Scene3DInstance::~Scene3DInstance() {
    cleanup();
    globalLightInfo.destroy();
    spotlights.destroy();
    pointLights.destroy();
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

void Scene3DInstance::init(DescriptorComponentStorageCache&) {
    // allocate memory
    createCameraBuffer();
    globalLightInfo.create(vulkanState, 1);
    spotlights.create(vulkanState, lgt::Scene3DLighting::MaxSpotLights);
    pointLights.create(vulkanState, lgt::Scene3DLighting::MaxPointLights);

    emptySpotShadowMap.create(vulkanState,
                              vk::Image::Type::Image2D,
                              vulkanState.findDepthFormat(),
                              VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                                  VK_IMAGE_USAGE_SAMPLED_BIT,
                              renderer.getSettings().getShadowMapResolution(),
                              VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                              0,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                              0,
                              VK_IMAGE_ASPECT_DEPTH_BIT);
    emptyPointShadowMap.create(vulkanState,
                               vk::Image::Type::Cubemap,
                               vulkanState.findDepthFormat(),
                               VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                                   VK_IMAGE_USAGE_SAMPLED_BIT,
                               renderer.getSettings().getShadowMapResolution(),
                               VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                               0,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                               0,
                               VK_IMAGE_ASPECT_DEPTH_BIT);

    auto commandBuffer = vulkanState.sharedCommandPool.createBuffer();
    emptySpotShadowMap.clearDepthAndPrepareForSampling(commandBuffer);
    emptyPointShadowMap.clearDepthAndPrepareForSampling(commandBuffer);
    commandBuffer.submit();

    // allocate descriptors
    allocateDescriptorSets();

    // create and configureWrite descriptors
    const std::uint32_t bufferWriteCount = Config::MaxConcurrentFrames * 4;
    SetWriteHelper setWriter;
    setWriter.hintWriteCount(descriptorSets.size() * bufferWriteCount);
    setWriter.hintBufferInfoCount(descriptorSets.size() * bufferWriteCount);

    // write descriptors
    for (std::uint32_t i = 0; i < Config::MaxSceneObservers; ++i) {
        for (std::uint32_t j = 0; j < Config::MaxConcurrentFrames; ++j) {
            const auto set = descriptorSets.getRaw(i, j);

            writeCameraDescriptor(setWriter, i, j);

            VkDescriptorBufferInfo& lightInfoBufferInfo = setWriter.getNewBufferInfo();
            lightInfoBufferInfo.buffer = globalLightInfo.gpuBufferHandle().getBuffer();
            lightInfoBufferInfo.offset = 0;
            lightInfoBufferInfo.range  = globalLightInfo.totalAlignedSize();

            VkWriteDescriptorSet& lightInfoWrite = setWriter.getNewSetWrite(set);
            lightInfoWrite.descriptorCount       = 1;
            lightInfoWrite.dstBinding            = 1;
            lightInfoWrite.dstArrayElement       = 0;
            lightInfoWrite.pBufferInfo           = &lightInfoBufferInfo;
            lightInfoWrite.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

            VkDescriptorBufferInfo& pointLightInfo = setWriter.getNewBufferInfo();
            pointLightInfo.buffer                  = pointLights.gpuBufferHandle().getBuffer();
            pointLightInfo.offset                  = 0;
            pointLightInfo.range                   = pointLights.totalAlignedSize();

            VkWriteDescriptorSet& pointLightWrite = setWriter.getNewSetWrite(set);
            pointLightWrite.descriptorCount       = 1;
            pointLightWrite.dstBinding            = 2;
            pointLightWrite.dstArrayElement       = 0;
            pointLightWrite.pBufferInfo           = &pointLightInfo;
            pointLightWrite.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

            VkDescriptorBufferInfo& spotlightInfo = setWriter.getNewBufferInfo();
            spotlightInfo.buffer                  = spotlights.gpuBufferHandle().getBuffer();
            spotlightInfo.offset                  = 0;
            spotlightInfo.range                   = spotlights.totalAlignedSize();

            VkWriteDescriptorSet& spotlightWrite = setWriter.getNewSetWrite(set);
            spotlightWrite.descriptorCount       = 1;
            spotlightWrite.dstBinding            = 3;
            spotlightWrite.dstArrayElement       = 0;
            spotlightWrite.pBufferInfo           = &spotlightInfo;
            spotlightWrite.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
    }
    setWriter.performWrite(vulkanState.device);

    updateShadowDescriptors(nullptr);

    // populate lighting with defaults
    globalLightInfo.fill(lgt::LightingDescriptor3D());
    spotlights.fill(lgt::SpotLight3D());
    pointLights.fill(lgt::PointLight3D());
    globalLightInfo.transferEveryFrame();
    spotlights.transferEveryFrame();
    pointLights.transferEveryFrame();

    bl::event::Dispatcher::subscribe(this);
}

void Scene3DInstance::updateShadowDescriptors(rg::GraphAsset* asset) {
    rgi::ShadowMapAsset* shadowMaps =
        asset ? dynamic_cast<rgi::ShadowMapAsset*>(&asset->asset.get()) : nullptr;
    if (!shadowMaps && asset) {
        BL_LOG_WARN << "Shadow descriptors requires ShadowMapAsset";
        return;
    }

    const std::uint32_t imageWriteCount =
        Config::MaxConcurrentFrames * (Config::MaxSpotShadows + Config::MaxPointShadows);
    SetWriteHelper setWriter;
    setWriter.hintWriteCount(descriptorSets.size() * imageWriteCount);
    setWriter.hintImageInfoCount(descriptorSets.size() * imageWriteCount);

    for (std::uint32_t i = 0; i < Config::MaxSceneObservers; ++i) {
        for (std::uint32_t j = 0; j < Config::MaxConcurrentFrames; ++j) {
            const auto set = descriptorSets.getRaw(i, j);

            // TODO - is this the one we want?
            VkSampler sampler = vulkanState.samplerCache.filteredRepeated();
            for (unsigned int i = 0; i < Config::MaxSpotShadows; ++i) {
                VkDescriptorImageInfo& imageInfo = setWriter.getNewImageInfo();
                imageInfo.imageLayout            = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = shadowMaps ? shadowMaps->getSpotShadowImage(i).getView() :
                                                   emptySpotShadowMap.getView();
                imageInfo.sampler   = sampler;

                VkWriteDescriptorSet& write = setWriter.getNewSetWrite(set);
                write.descriptorCount       = 1;
                write.dstBinding            = 4;
                write.dstArrayElement       = i;
                write.pImageInfo            = &imageInfo;
                write.descriptorType        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            }

            for (unsigned int i = 0; i < Config::MaxPointShadows; ++i) {
                VkDescriptorImageInfo& imageInfo = setWriter.getNewImageInfo();
                imageInfo.imageLayout            = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = shadowMaps ? shadowMaps->getPointShadowImage(i).getView() :
                                                   emptyPointShadowMap.getView();
                imageInfo.sampler   = sampler;

                VkWriteDescriptorSet& write = setWriter.getNewSetWrite(set);
                write.descriptorCount       = 1;
                write.dstBinding            = 5;
                write.dstArrayElement       = i;
                write.pImageInfo            = &imageInfo;
                write.descriptorType        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            }
        }
    }
    setWriter.performWrite(vulkanState.device);
}

void Scene3DInstance::observe(const event::SceneGraphAssetCreated& event) {
    if (event.scene == static_cast<Scene*>(owner)) { updateShadowDescriptors(event.asset); }
}

bool Scene3DInstance::allocateObject(ecs::Entity, scene::Key) {
    // n/a
    return true;
}

void Scene3DInstance::handleFrameStart() {
    // noop
}

} // namespace ds
} // namespace rc
} // namespace bl
