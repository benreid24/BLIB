#include <BLIB/Render/Descriptors/Builtin/SlideshowInstance.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
SlideshowInstance::SlideshowInstance(engine::Engine& engine,
                                     VkDescriptorSetLayout descriptorSetLayout)
: DescriptorSetInstance(Bindless, SpeedAgnostic)
, registry(engine.ecs())
, vulkanState(engine.renderer().vulkanState())
, descriptorSetLayout(descriptorSetLayout)
, staticDescriptorSets(engine.renderer().vulkanState())
, dynamicDescriptorSets(engine.renderer().vulkanState())
, refreshRequired(0)
, lastFrameUploaded(0)
, lastOffsetUploaded(0)
, nextPlayerIndex(0) {
    staticDescriptorSets.emptyInit(vulkanState);
    dynamicDescriptorSets.emptyInit(vulkanState);
}

void SlideshowInstance::init(DescriptorComponentStorageCache& storageCache) {
    objToAnimIndex = storageCache.getComponentStorage<com::Animation2D,
                                                      std::uint32_t,
                                                      buf::DynamicSSBO<std::uint32_t>,
                                                      buf::StaticSSBO<std::uint32_t>>();
}

void SlideshowInstance::bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                        std::uint32_t setIndex, UpdateSpeed updateFreq) const {
    const auto set = updateFreq == UpdateSpeed::Static ? staticDescriptorSets.current().getSet() :
                                                         dynamicDescriptorSets.current().getSet();
    vkCmdBindDescriptorSets(ctx.getCommandBuffer(),
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout,
                            setIndex,
                            1,
                            &set,
                            0,
                            nullptr);
}

void SlideshowInstance::bindForObject(scene::SceneRenderContext&, VkPipelineLayout, std::uint32_t,
                                      scene::Key) const {
    // noop
}

bool SlideshowInstance::allocateObject(ecs::Entity entity, scene::Key key) {
    com::Animation2D* anim = animComponents->get(entity);
    if (!anim) {
        BL_LOG_ERROR << "Failed to add animation: Missing Animation component";
        return false;
    }
    if (!ensurePlayerPresent(anim->playerEntity, anim->playerIndex)) {
        BL_LOG_ERROR << "Failed to add animation: Could not validate player";
        return false;
    }
    if (!objToAnimIndex->allocateObject(entity, key)) {
        BL_LOG_ERROR << "Failed to add animation: Could not link descriptor";
        return false;
    }
    return false;
}

void SlideshowInstance::releaseObject(ecs::Entity entity, scene::Key key) {
    objToAnimIndex->releaseObject(entity, key);
    // TODO - ref count animations and free unused data?
}

void SlideshowInstance::handleFrameStart() {
    // update player indices where required
    animComponents->forEach([this](ecs::Entity, com::Animation2D& anim) {
        if (anim.isDirty()) {
            if (!ensurePlayerPresent(anim.playerEntity, anim.playerIndex)) {
                BL_LOG_ERROR << "Animation updated to invalid player";
            }
        }
    });
    // TODO - we can avoid above loop by adding hook into dirty calls somehow

    // rebuild descriptors if buffers changed
    if (refreshRequired != 0 || objToAnimIndex->dynamicDescriptorUpdateRequired() ||
        objToAnimIndex->staticDescriptorUpdateRequired()) {
        updateDescriptorSets();
    }

    // upload static buffers if new data
    if (framesSSBO.size() > lastFrameUploaded) {
        framesSSBO.transferRange(lastFrameUploaded, framesSSBO.size() - lastFrameUploaded);
        lastFrameUploaded = framesSSBO.size();
    }
    if (frameOffsetSSBO.size() > lastOffsetUploaded) {
        frameOffsetSSBO.transferRange(lastOffsetUploaded,
                                      frameOffsetSSBO.size() - lastOffsetUploaded);
        lastOffsetUploaded = frameOffsetSSBO.size();
    }

    // always upload all play indices
    playerCurrentFrameSSBO.transferAll();
    // TODO - consider component based storage w/o static/dynamic split for this kind of use case

    refreshRequired = refreshRequired >> 1;
}

bool SlideshowInstance::ensurePlayerPresent(ecs::Entity ent, std::uint32_t& playerIndex) {
    // return existing index if we already have the player loaded
    const auto it = playerIndices.find(ent);
    if (it != playerIndices.end()) {
        playerIndex = it->second;
        return true;
    }

    // validate
    com::Animation2DPlayer* player = registry.getComponent<com::Animation2DPlayer>(ent);
    if (!player) {
        BL_LOG_ERROR << "Failed to find animation player for entity: " << ent;
        return false;
    }
    if (!player->animation) {
        BL_LOG_ERROR << "Animation player does not refer to animation";
        return false;
    }
    if (!gfx::a2d::AnimationData::isValidSlideshow(*player->animation)) {
        BL_LOG_ERROR << "Animation must be slideshow";
        return false;
    }

    // add mapping
    // TODO - extra mapping to dedupe anim frames referred to by multiple players
    const std::uint32_t offset = framesSSBO.size();
    const std::uint32_t index  = nextPlayerIndex++;
    playerIndices[ent]         = index;
    playerIndex                = index;

    // add frame offset and current frame to SSBO's
    frameOffsetSSBO.ensureSize(index + 1);
    playerCurrentFrameSSBO.ensureSize(index + 1);
    frameOffsetSSBO[index]        = offset;
    playerCurrentFrameSSBO[index] = player->currentFrame;

    // add frames to framesSSBO
    framesSSBO.ensureSize(framesSSBO.size() + player->animation->frameCount());
    for (std::size_t i = 0; i < player->animation->frameCount(); ++i) {
        const auto& src = player->animation->getFrame(i).shards.front();
        auto& frame     = framesSSBO[offset + i];
        const sf::FloatRect tex(src.source);
        frame.opacity      = static_cast<float>(src.alpha) / 255.f;
        frame.texCoords[0] = {tex.left, tex.top};
        frame.texCoords[1] = {tex.left + tex.width, tex.top};
        frame.texCoords[2] = {tex.left + tex.width, tex.top + tex.height};
        frame.texCoords[3] = {tex.left, tex.top + tex.height};
    }

    // mark that descriptors need to be reset
    refreshRequired = 0x1 << Config::MaxConcurrentFrames;

    return true;
}

void SlideshowInstance::refreshPlayerPointers() {
    for (const auto& pair : playerIndices) {
        com::Animation2DPlayer* player = registry.getComponent<com::Animation2DPlayer>(pair.first);
        if (player) { player->framePayload = &playerCurrentFrameSSBO[pair.second]; }
        else { BL_LOG_ERROR << "Animation player not found for entity: " << pair.first; }
    }
}

void SlideshowInstance::updateDescriptorSets() {
    VkWriteDescriptorSet setWrites[8]{};

    // (re)allocate the descriptor sets
    dynamicDescriptorSets.current().allocate(descriptorSetLayout);
    staticDescriptorSets.current().allocate(descriptorSetLayout);

    // object to player index mapping (binding 0)
    VkDescriptorBufferInfo objMapWriteStatic{};
    objMapWriteStatic.buffer = objToAnimIndex->getStaticBuffer().gpuBufferHandle().getBuffer();
    objMapWriteStatic.offset = 0;
    objMapWriteStatic.range  = objToAnimIndex->getStaticBuffer().getTotalRange();

    setWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[0].descriptorCount = 1;
    setWrites[0].dstBinding      = 0;
    setWrites[0].dstArrayElement = 0;
    setWrites[0].dstSet          = staticDescriptorSets.current().getSet();
    setWrites[0].pBufferInfo     = &objMapWriteStatic;
    setWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    VkDescriptorBufferInfo objMapWriteDynamic{};
    objMapWriteDynamic.buffer =
        objToAnimIndex->getDynamicBuffer().gpuBufferHandles().current().getBuffer();
    objMapWriteDynamic.offset = 0;
    objMapWriteDynamic.range  = objToAnimIndex->getDynamicBuffer().getTotalRange();

    setWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[1].descriptorCount = 1;
    setWrites[1].dstBinding      = 0;
    setWrites[1].dstArrayElement = 0;
    setWrites[1].dstSet          = dynamicDescriptorSets.current().getSet();
    setWrites[1].pBufferInfo     = &objMapWriteDynamic;
    setWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    // frame offset (binding 1)
    VkDescriptorBufferInfo frameOffsetWrite{};
    frameOffsetWrite.buffer = frameOffsetSSBO.gpuBufferHandle().getBuffer();
    frameOffsetWrite.offset = 0;
    frameOffsetWrite.range  = frameOffsetSSBO.getTotalRange();

    setWrites[2].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[2].descriptorCount = 1;
    setWrites[2].dstBinding      = 1;
    setWrites[2].dstArrayElement = 0;
    setWrites[2].dstSet          = dynamicDescriptorSets.current().getSet();
    setWrites[2].pBufferInfo     = &frameOffsetWrite;
    setWrites[2].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    setWrites[3]                 = setWrites[2];
    setWrites[3].dstSet          = staticDescriptorSets.current().getSet();

    // current frames (binding 2)
    VkDescriptorBufferInfo currentFrameWrite{};
    currentFrameWrite.buffer = playerCurrentFrameSSBO.gpuBufferHandles().current().getBuffer();
    currentFrameWrite.offset = 0;
    currentFrameWrite.range  = playerCurrentFrameSSBO.getTotalRange();

    setWrites[4].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[4].descriptorCount = 1;
    setWrites[4].dstBinding      = 2;
    setWrites[4].dstArrayElement = 0;
    setWrites[4].dstSet          = dynamicDescriptorSets.current().getSet();
    setWrites[4].pBufferInfo     = &currentFrameWrite;
    setWrites[4].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    setWrites[5]                 = setWrites[4];
    setWrites[5].dstSet          = staticDescriptorSets.current().getSet();

    // frame data (binding 3)
    VkDescriptorBufferInfo frameDataWrite{};
    frameDataWrite.buffer = framesSSBO.gpuBufferHandle().getBuffer();
    frameDataWrite.offset = 0;
    frameDataWrite.range  = framesSSBO.getTotalRange();

    setWrites[6].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[6].descriptorCount = 1;
    setWrites[6].dstBinding      = 3;
    setWrites[6].dstArrayElement = 0;
    setWrites[6].dstSet          = dynamicDescriptorSets.current().getSet();
    setWrites[6].pBufferInfo     = &frameDataWrite;
    setWrites[6].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    setWrites[7]                 = setWrites[6];
    setWrites[7].dstSet          = staticDescriptorSets.current().getSet();

    // perform write
    vkUpdateDescriptorSets(vulkanState.device, std::size(setWrites), setWrites, 0, nullptr);
}

} // namespace ds
} // namespace rc
} // namespace bl
