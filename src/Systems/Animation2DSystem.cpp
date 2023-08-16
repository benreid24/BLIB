#include <BLIB/Systems/Animation2DSystem.hpp>

#include <BLIB/Engine.hpp>
#include <BLIB/Render.hpp>

namespace bl
{
namespace sys
{
Animation2DSystem::Animation2DSystem(rc::Renderer& renderer)
: device(renderer.vulkanState().device)
, renderer(renderer)
, players(nullptr)
, nextSlideshowPlayerIndex(0)
, slideshowDescriptorSets(renderer.vulkanState())
, slideshowRefreshRequired(0)
, lastSlideshowFrameUploaded(0)
, lastSlideshowOffsetUploaded(0) {
    slideshowDescriptorSets.emptyInit(renderer.vulkanState());
}

void Animation2DSystem::bindSlideshowSet(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                                         std::uint32_t setIndex) {
    VkDescriptorSet set = slideshowDescriptorSets.current().getSet();
    vkCmdBindDescriptorSets(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, setIndex, 1, &set, 0, nullptr);
}

void Animation2DSystem::init(engine::Engine& engine) {
    descriptorLayout = renderer.descriptorFactoryCache()
                           .getOrCreateFactory<rc::ds::SlideshowFactory>()
                           ->getDescriptorLayout();

    players = &engine.ecs().getAllComponents<com::Animation2DPlayer>();

    // TODO - pre-allocate for some animations?
}

void Animation2DSystem::update(std::mutex&, float dt) {
    // play animations
    players->forEach([dt](ecs::Entity, com::Animation2DPlayer& player) { player.update(dt); });

    // perform slideshow uploads
    if (slideshowFramesSSBO.size() > lastSlideshowFrameUploaded) {
        slideshowFramesSSBO.transferRange(lastSlideshowFrameUploaded,
                                          slideshowFramesSSBO.size() - lastSlideshowFrameUploaded);
        lastSlideshowFrameUploaded = slideshowFramesSSBO.size();
    }
    if (slideshowFrameOffsetSSBO.size() > lastSlideshowOffsetUploaded) {
        slideshowFrameOffsetSSBO.transferRange(lastSlideshowOffsetUploaded,
                                               slideshowFrameOffsetSSBO.size() -
                                                   lastSlideshowOffsetUploaded);
        lastSlideshowOffsetUploaded = slideshowFrameOffsetSSBO.size();
    }
    slideshowPlayerCurrentFrameSSBO.transferAll(); // always upload all play indices

    // update descriptors
    if (slideshowRefreshRequired != 0) {
        --slideshowRefreshRequired;
        updateSlideshowDescrptorSets();
    }
}

void Animation2DSystem::observe(const ecs::event::ComponentAdded<com::Animation2DPlayer>& event) {
    if (gfx::a2d::AnimationData::isValidSlideshow(*event.component.animation)) {
        doSlideshowAdd(event.entity, event.component);
    }
    else {
        // TODO - handle non-slideshow animations
    }
}

void Animation2DSystem::observe(const ecs::event::ComponentRemoved<com::Animation2DPlayer>& event) {
    // TODO - check ref counts and remove data
}

void Animation2DSystem::doSlideshowAdd(ecs::Entity playerEntity, com::Animation2DPlayer& player) {
    // determine frame offset and player index
    const auto it              = slideshowFrameMap.find(player.animation.get());
    const bool uploadFrames    = it == slideshowFrameMap.end();
    const std::uint32_t offset = uploadFrames ? slideshowFramesSSBO.size() : it->second;
    const std::uint32_t index  = nextSlideshowPlayerIndex++;
    player.playerIndex         = index;

    // add frame offset and current frame to SSBO's
    slideshowFrameOffsetSSBO.ensureSize(index + 1);
    slideshowPlayerCurrentFrameSSBO.ensureSize(index + 1);
    slideshowFrameOffsetSSBO[index]        = offset;
    slideshowPlayerCurrentFrameSSBO[index] = player.currentFrame;

    // add frames to ssbo if requierd
    if (uploadFrames) {
        slideshowFrameMap[player.animation.get()] = offset;
        slideshowFramesSSBO.ensureSize(slideshowFramesSSBO.size() + player.animation->frameCount());
        for (std::size_t i = 0; i < player.animation->frameCount(); ++i) {
            const auto& src = player.animation->getFrame(i).shards.front();
            auto& frame     = slideshowFramesSSBO[offset + i];
            const sf::FloatRect tex(src.source);
            frame.opacity      = static_cast<float>(src.alpha) / 255.f;
            frame.texCoords[0] = {tex.left, tex.top};
            frame.texCoords[1] = {tex.left + tex.width, tex.top};
            frame.texCoords[2] = {tex.left + tex.width, tex.top + tex.height};
            frame.texCoords[3] = {tex.left, tex.top + tex.height};
        }
    }

    // mark that descriptors need to be reset
    slideshowRefreshRequired = rc::Config::MaxConcurrentFrames;
}

void Animation2DSystem::updateSlideshowDescrptorSets() {
    VkWriteDescriptorSet setWrites[3]{};

    // (re)allocate the descriptor sets
    slideshowDescriptorSets.current().allocate(descriptorLayout);

    // frame offset (binding 0)
    VkDescriptorBufferInfo frameOffsetWrite{};
    frameOffsetWrite.buffer = slideshowFrameOffsetSSBO.gpuBufferHandle().getBuffer();
    frameOffsetWrite.offset = 0;
    frameOffsetWrite.range  = slideshowFrameOffsetSSBO.getTotalRange();

    setWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[0].descriptorCount = 1;
    setWrites[0].dstBinding      = 0;
    setWrites[0].dstArrayElement = 0;
    setWrites[0].dstSet          = slideshowDescriptorSets.current().getSet();
    setWrites[0].pBufferInfo     = &frameOffsetWrite;
    setWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    // current frames (binding 1)
    VkDescriptorBufferInfo currentFrameWrite{};
    currentFrameWrite.buffer =
        slideshowPlayerCurrentFrameSSBO.gpuBufferHandles().current().getBuffer();
    currentFrameWrite.offset = 0;
    currentFrameWrite.range  = slideshowPlayerCurrentFrameSSBO.getTotalRange();

    setWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[1].descriptorCount = 1;
    setWrites[1].dstBinding      = 1;
    setWrites[1].dstArrayElement = 0;
    setWrites[1].dstSet          = slideshowDescriptorSets.current().getSet();
    setWrites[1].pBufferInfo     = &currentFrameWrite;
    setWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    // frame data (binding 2)
    VkDescriptorBufferInfo frameDataWrite{};
    frameDataWrite.buffer = slideshowFramesSSBO.gpuBufferHandle().getBuffer();
    frameDataWrite.offset = 0;
    frameDataWrite.range  = slideshowFramesSSBO.getTotalRange();

    setWrites[2].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[2].descriptorCount = 1;
    setWrites[2].dstBinding      = 2;
    setWrites[2].dstArrayElement = 0;
    setWrites[2].dstSet          = slideshowDescriptorSets.current().getSet();
    setWrites[2].pBufferInfo     = &frameDataWrite;
    setWrites[2].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    // perform write
    vkUpdateDescriptorSets(device, std::size(setWrites), setWrites, 0, nullptr);
}

} // namespace sys
} // namespace bl
