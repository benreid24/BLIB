#include <BLIB/Render/Resources/GlobalDescriptors.hpp>

#include <BLIB/Containers/StaticVector.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Resources/MaterialPool.hpp>
#include <BLIB/Render/Resources/TexturePool.hpp>
#include <BLIB/Render/Settings.hpp>

namespace bl
{
namespace rc
{
namespace res
{
GlobalDescriptors::GlobalDescriptors(engine::Engine& engine, Renderer& renderer,
                                     TexturePool& texturePool, MaterialPool& materialPool)
: engine(engine)
, renderer(renderer)
, texturePool(texturePool)
, materialPool(materialPool)
, accumulatedTimings{} {}

void GlobalDescriptors::bindDescriptors(VkCommandBuffer cb, VkPipelineBindPoint bindPoint,
                                        VkPipelineLayout pipelineLayout, std::uint32_t setIndex,
                                        bool forRt) {
    const VkDescriptorSet ds = forRt ? rtDescriptorSets.current() : descriptorSets.current();
    vkCmdBindDescriptorSets(cb, bindPoint, pipelineLayout, setIndex, 1, &ds, 0, nullptr);
}

void GlobalDescriptors::init() {
    auto& vulkanState = renderer.vulkanState();

    VkDescriptorSetLayoutBinding setBindings[6] = {texturePool.getTextureLayoutBinding(),
                                                   materialPool.getLayoutBinding(),
                                                   texturePool.getCubemapLayoutBinding(),
                                                   {},
                                                   {},
                                                   {}};

    VkDescriptorSetLayoutBinding& frameDataBinding = setBindings[3];
    frameDataBinding.descriptorCount               = 1;
    frameDataBinding.binding                       = 3;
    frameDataBinding.stageFlags                    = VK_SHADER_STAGE_ALL;
    frameDataBinding.descriptorType                = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    frameDataBinding.pImmutableSamplers            = 0;

    VkDescriptorSetLayoutBinding& settingsBinding = setBindings[4];
    settingsBinding.descriptorCount               = 1;
    settingsBinding.binding                       = 4;
    settingsBinding.stageFlags                    = VK_SHADER_STAGE_ALL;
    settingsBinding.descriptorType                = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    settingsBinding.pImmutableSamplers            = 0;

    VkDescriptorSetLayoutBinding& dynamicSettingsBinding = setBindings[5];
    dynamicSettingsBinding.descriptorCount               = 1;
    dynamicSettingsBinding.binding                       = 5;
    dynamicSettingsBinding.stageFlags                    = VK_SHADER_STAGE_ALL;
    dynamicSettingsBinding.descriptorType                = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    dynamicSettingsBinding.pImmutableSamplers            = 0;

    // create descriptor layout
    constexpr std::size_t NBindings = std::size(setBindings);
    VkDescriptorSetLayoutCreateInfo descriptorCreateInfo{};
    descriptorCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorCreateInfo.bindingCount = NBindings;
    descriptorCreateInfo.pBindings    = setBindings;
    if (VK_SUCCESS !=
        vkCreateDescriptorSetLayout(
            vulkanState.device, &descriptorCreateInfo, nullptr, &descriptorSetLayout)) {
        throw std::runtime_error("Failed to create texture pool descriptor set layout");
    }

    // create descriptor pool
    ctr::StaticVector<VkDescriptorPoolSize, 10> poolSizes;
    for (std::size_t i = 0; i < NBindings; ++i) {
        VkDescriptorPoolSize* poolSize = nullptr;
        for (VkDescriptorPoolSize& ps : poolSizes) {
            if (ps.type == setBindings[i].descriptorType) {
                poolSize = &ps;
                break;
            }
        }
        if (!poolSize) { poolSize = &poolSizes.emplace_back(VkDescriptorPoolSize{}); }
        poolSize->type = setBindings[i].descriptorType;
        poolSize->descriptorCount +=
            setBindings[i].descriptorCount * cfg::Limits::MaxConcurrentFrames * 2;
    }

    VkDescriptorPoolCreateInfo poolCreate{};
    poolCreate.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreate.maxSets       = 2 * cfg::Limits::MaxConcurrentFrames;
    poolCreate.poolSizeCount = poolSizes.size();
    poolCreate.pPoolSizes    = poolSizes.data();
    if (VK_SUCCESS !=
        vkCreateDescriptorPool(vulkanState.device, &poolCreate, nullptr, &descriptorPool)) {
        throw std::runtime_error("Failed to create texture descriptor pool");
    }

    // allocate descriptor set
    VkDescriptorSet allocatedSets[2 * cfg::Limits::MaxConcurrentFrames];
    std::array<VkDescriptorSetLayout, 2 * cfg::Limits::MaxConcurrentFrames> setLayouts;
    setLayouts.fill(descriptorSetLayout);

    VkDescriptorSetAllocateInfo setAlloc{};
    setAlloc.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAlloc.descriptorPool     = descriptorPool;
    setAlloc.descriptorSetCount = setLayouts.size();
    setAlloc.pSetLayouts        = setLayouts.data();
    if (VK_SUCCESS != vkAllocateDescriptorSets(vulkanState.device, &setAlloc, allocatedSets)) {
        throw std::runtime_error("Failed to allocate texture descriptor set");
    }

    // write descriptor set with texture and material pools
    unsigned int i = 0;
    descriptorSets.init(vulkanState, [&i, &allocatedSets](auto& set) { set = allocatedSets[i++]; });
    rtDescriptorSets.init(vulkanState,
                          [&i, &allocatedSets](auto& set) { set = allocatedSets[i++]; });

    texturePool.init(descriptorSets, rtDescriptorSets);
    materialPool.init(descriptorSets, rtDescriptorSets);

    // write descriptor set with settings uniform
    auto& pool              = renderer.getGlobalShaderResources();
    settingsResource        = pool.getShaderResourceWithKey(sri::SettingsUniformResourceKey);
    frameTimeResource       = pool.getShaderResourceWithKey(sri::FrameDataUniformResourceKey);
    dynamicSettingsResource = pool.getShaderResourceWithKey(sri::DynamicSettingsUniformResourceKey);
    ds::SetWriteHelper writer;
    const auto writeGlobalsSet = [this, &writer](VkDescriptorSet set, std::uint32_t i, bool forRt) {
        // frame data
        auto& frameDataBufferInfo  = writer.getNewBufferInfo();
        frameDataBufferInfo.buffer = frameTimeResource->getBuffer().getRawBuffer(i);
        frameDataBufferInfo.offset = 0;
        frameDataBufferInfo.range  = frameTimeResource->getBuffer().getAlignedElementSize();

        auto& frameDataWrite           = writer.getNewSetWrite(set);
        frameDataWrite.descriptorCount = 1;
        frameDataWrite.dstBinding      = 3;
        frameDataWrite.dstArrayElement = 0;
        frameDataWrite.pBufferInfo     = &frameDataBufferInfo;
        frameDataWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        // settings
        auto& settingsBufferInfo  = writer.getNewBufferInfo();
        settingsBufferInfo.buffer = settingsResource->getBuffer().getCurrentFrameRawBuffer();
        settingsBufferInfo.offset =
            forRt ? settingsResource->getBuffer().getAlignedElementSize() : 0;
        settingsBufferInfo.range  = settingsResource->getBuffer().getAlignedElementSize();

        auto& settingsWrite           = writer.getNewSetWrite(set);
        settingsWrite.descriptorCount = 1;
        settingsWrite.dstBinding      = 4;
        settingsWrite.dstArrayElement = 0;
        settingsWrite.pBufferInfo     = &settingsBufferInfo;
        settingsWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        // dynamic settings
        auto& dynamicSettingsBufferInfo = writer.getNewBufferInfo();
        dynamicSettingsBufferInfo.buffer =
            dynamicSettingsResource->getBuffer().getCurrentFrameBuffer().getBuffer();
        dynamicSettingsBufferInfo.offset = 0;
        dynamicSettingsBufferInfo.range  = dynamicSettingsResource->getBuffer().getTotalAlignedSize();

        auto& dynamicSettingsWrite           = writer.getNewSetWrite(set);
        dynamicSettingsWrite.descriptorCount = 1;
        dynamicSettingsWrite.dstBinding      = 5;
        dynamicSettingsWrite.dstArrayElement = 0;
        dynamicSettingsWrite.pBufferInfo     = &dynamicSettingsBufferInfo;
        dynamicSettingsWrite.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    };

    settingsResource->getBuffer().setCopyFullRange(true);
    dynamicSettingsResource->getBuffer().setCopyFullRange(true);

    frameTimeResource->getBuffer().transferEveryFrame();
    frameTimeResource->getBuffer().setCopyFullRange(true);

    i = 0;
    descriptorSets.visit(
        [&writeGlobalsSet, &i](VkDescriptorSet set) { writeGlobalsSet(set, i++, false); });
    i = 0;
    rtDescriptorSets.visit(
        [&writeGlobalsSet, &i](VkDescriptorSet set) { writeGlobalsSet(set, i++, true); });
    writer.performWrite(vulkanState.device);
    updateSettings(renderer.getSettings());
}

void GlobalDescriptors::cleanup() {
    texturePool.cleanup();
    materialPool.cleanup();

    vkDestroyDescriptorPool(renderer.vulkanState().device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(renderer.vulkanState().device, descriptorSetLayout, nullptr);
}

void GlobalDescriptors::onFrameStart() {
    texturePool.onFrameStart(
        descriptorWriter, descriptorSets.current(), rtDescriptorSets.current());
    materialPool.onFrameStart();
    descriptorWriter.performWrite(renderer.vulkanState().device);
    accumulatedTimings.realFrameDt = frameTimer.restart().asSeconds();
    accumulatedTimings.frameDt     = accumulatedTimings.realFrameDt * engine.getTimeScale();
    frameTimer.restart();
    frameTimeResource->getBuffer().writeDirect(&accumulatedTimings, 1, 0);
    accumulatedTimings = {};
}

void GlobalDescriptors::notifyUpdateTick(float dt, float realDt, float residual,
                                         float realResidual) {
    accumulatedTimings.dt += dt;
    accumulatedTimings.realDt += realDt;
    accumulatedTimings.residual     = residual;
    accumulatedTimings.realResidual = realResidual;
}

void GlobalDescriptors::updateSettings(const Settings& settings) {
    // reset current hdr exposure level
    dynamicSettingsResource->getBuffer()[0].currentHdrExposure = settings.getExposureFactor();
    dynamicSettingsResource->getBuffer().queueTransfer();

    // populate global settings
    settingsResource->getBuffer()[0].gamma       = settings.getGamma();
    settingsResource->getBuffer()[0].hdrSettings = settings.getAutoHDRSettings();

    // handle render texture specific settings
    settingsResource->getBuffer()[1]       = settingsResource->getBuffer()[0];
    settingsResource->getBuffer()[1].gamma = 1.f; // cancel out gamma correction
    settingsResource->getBuffer().queueTransfer();
}

} // namespace res
} // namespace rc
} // namespace bl