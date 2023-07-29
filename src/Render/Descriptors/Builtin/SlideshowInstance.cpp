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
    // TODO
}

void SlideshowInstance::bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                      std::uint32_t setIndex, scene::Key objectKey) const {
    // TODO
}

bool SlideshowInstance::allocateObject(ecs::Entity entity, scene::Key key) {
    // TODO
    return false;
}

void SlideshowInstance::releaseObject(ecs::Entity entity, scene::Key key) {
    // TODO
}

void SlideshowInstance::handleFrameStart() {
    // TODO
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
    if (!gfx::a2d::AnimationData::isValidSlideshow(*player->animation)) {
        BL_LOG_ERROR << "Animation must be slideshow";
        return false;
    }

    // add mapping
    const std::uint32_t offset = framesSSBO.size();
    const std::uint32_t index  = nextPlayerIndex++;
    playerIndices[ent]         = index;

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
    // TODO - refresh descriptors if required. transfer buffers using lastUploaded
    // TODO - examine dirty range of animation2d and update indices on component
}

} // namespace ds
} // namespace rc
} // namespace bl
