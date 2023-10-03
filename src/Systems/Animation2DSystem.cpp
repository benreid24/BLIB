#include <BLIB/Systems/Animation2DSystem.hpp>

#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/Engine.hpp>
#include <BLIB/Render.hpp>

namespace bl
{
namespace sys
{
namespace
{
constexpr std::uint32_t InitialSlideshowFrameCapacity = 128;
}

Animation2DSystem::Animation2DSystem(rc::Renderer& renderer)
: renderer(renderer)
, players(nullptr)
, slideshowFrameRangeAllocator(InitialSlideshowFrameCapacity)
, slideshowDescriptorSets(renderer.vulkanState())
, slideshowRefreshRequired(rc::Config::MaxConcurrentFrames)
, slideshowLastFrameUpdated(255) {
    slideshowDescriptorSets.emptyInit(renderer.vulkanState());
}

void Animation2DSystem::cleanup() {
    event::Dispatcher::unsubscribe(this);
    slideshowDescriptorSets.cleanup([](rc::vk::DescriptorSet& ds) { ds.release(); });
    slideshowFramesSSBO.destroy();
    slideshowFrameOffsetSSBO.destroy();
    slideshowPlayerCurrentFrameSSBO.destroy();
    vertexAnimationData.clear();
}

void Animation2DSystem::bindSlideshowSet(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                                         std::uint32_t setIndex) {
    ensureSlideshowDescriptorsUpdated();
    VkDescriptorSet set = slideshowDescriptorSets.current().getSet();
    vkCmdBindDescriptorSets(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, setIndex, 1, &set, 0, nullptr);
}

void Animation2DSystem::init(engine::Engine& engine) {
    descriptorLayout = renderer.descriptorFactoryCache()
                           .getOrCreateFactory<rc::ds::SlideshowFactory>()
                           ->getDescriptorLayout();

    players    = &engine.ecs().getAllComponents<com::Animation2DPlayer>();
    vertexPool = &engine.ecs().getAllComponents<com::Animation2D>();

    slideshowFramesSSBO.create(renderer.vulkanState(), InitialSlideshowFrameCapacity);
    slideshowFrameOffsetSSBO.create(renderer.vulkanState(), 32);
    slideshowPlayerCurrentFrameSSBO.create(renderer.vulkanState(), 32);

    event::Dispatcher::subscribe(this);
}

void Animation2DSystem::update(std::mutex&, float dt) {
    // play animations
    players->forEach([dt](ecs::Entity, com::Animation2DPlayer& player) { player.update(dt); });

    // perform slideshow uploads
    if (slideshowFrameUploadRange.needsUpload()) {
        slideshowFramesSSBO.transferRange(slideshowFrameUploadRange.start,
                                          slideshowFrameUploadRange.size);
        slideshowFrameUploadRange.reset();
    }
    if (slideshowOffsetUploadRange.needsUpload()) {
        slideshowFrameOffsetSSBO.transferRange(slideshowOffsetUploadRange.start,
                                               slideshowOffsetUploadRange.size);
        slideshowOffsetUploadRange.reset();
    }
    slideshowPlayerCurrentFrameSSBO.transferAll(); // always upload all play indices

    // sync vertex animation draw parameters
    vertexPool->forEach([](ecs::Entity, com::Animation2D& anim) {
        if (anim.player && anim.sceneRef.object) {
            const VertexAnimation& data  = *static_cast<VertexAnimation*>(anim.systemHandle);
            const auto& frame            = data.frameToIndices[anim.player->currentFrame];
            anim.drawParams.vertexOffset = frame.vertexStart;
            anim.drawParams.indexOffset  = frame.indexStart;
            anim.drawParams.indexCount   = frame.indexCount;
            anim.syncDrawParamsToScene();
        }
    });
}

void Animation2DSystem::ensureSlideshowDescriptorsUpdated() {
    if (slideshowRefreshRequired != 0) {
        if (renderer.vulkanState().currentFrameIndex() != slideshowLastFrameUpdated) {
            slideshowLastFrameUpdated = renderer.vulkanState().currentFrameIndex();
            --slideshowRefreshRequired;
            updateSlideshowDescriptorSets();
        }
    }
}

void Animation2DSystem::observe(const ecs::event::ComponentAdded<com::Animation2DPlayer>& event) {
    if (event.component.animation->isSlideshow()) { doSlideshowAdd(event.entity, event.component); }
    else { doNonSlideshowCreate(event.component); }
}

void Animation2DSystem::observe(const ecs::event::ComponentRemoved<com::Animation2DPlayer>& event) {
    if (event.component.animation->isSlideshow()) {
        doSlideshowFree(event.entity, event.component);
    }
    else { tryFreeVertexData(event.component); }
}

void Animation2DSystem::observe(const ecs::event::ComponentRemoved<com::Animation2D>& event) {
    doNonSlideshowRemove(event.component);
}

void Animation2DSystem::doSlideshowAdd(ecs::Entity playerEntity, com::Animation2DPlayer& player) {
    // determine frame offset and player index
    const auto it           = slideshowFrameMap.find(player.animation.get());
    const bool uploadFrames = it == slideshowFrameMap.end();
    const std::uint32_t offset =
        uploadFrames ?
            slideshowFrameRangeAllocator.alloc(player.animation->frameCount()).range.start :
            it->second;
    const std::uint32_t index = slideshowPlayerIds.allocate();
    player.playerIndex        = index;

    // add frame offset and current frame to SSBO's
    slideshowFrameOffsetSSBO.ensureSize(index + 1);
    if (slideshowPlayerCurrentFrameSSBO.ensureSize(index + 1)) {
        players->forEach([this](ecs::Entity, com::Animation2DPlayer& player) {
            slideshowPlayerCurrentFrameSSBO.assignRef(player.framePayload, player.playerIndex);
        });
    }
    else { slideshowPlayerCurrentFrameSSBO.assignRef(player.framePayload, player.playerIndex); }
    slideshowFrameOffsetSSBO[index]        = offset;
    slideshowPlayerCurrentFrameSSBO[index] = player.currentFrame;
    slideshowOffsetUploadRange.addRange(index, 1);

    // add frames to SSBO if required
    if (uploadFrames) {
        slideshowFrameMap[player.animation.get()] = offset;
        slideshowFramesSSBO.ensureSize(offset + player.animation->frameCount());
        for (std::size_t i = 0; i < player.animation->frameCount(); ++i) {
            const auto& src          = player.animation->getFrame(i).shards.front();
            auto& frame              = slideshowFramesSSBO[offset + i];
            const sf::FloatRect& tex = src.normalizedSource;
            frame.opacity            = static_cast<float>(src.alpha) / 255.f;
            frame.texCoords[0]       = {tex.left, tex.top};
            frame.texCoords[1]       = {tex.left + tex.width, tex.top};
            frame.texCoords[2]       = {tex.left + tex.width, tex.top + tex.height};
            frame.texCoords[3]       = {tex.left, tex.top + tex.height};
        }
        slideshowFrameUploadRange.addRange(offset, player.animation->frameCount());
    }
    slideshowDataRefCounts[player.animation.get()] += 1;

    // mark that descriptors need to be reset
    slideshowRefreshRequired = rc::Config::MaxConcurrentFrames;
}

void Animation2DSystem::doSlideshowFree(ecs::Entity playerEntity,
                                        const com::Animation2DPlayer& player) {
    slideshowPlayerIds.release(player.playerIndex);

    const auto refCountIt = slideshowDataRefCounts.find(player.animation.get());
    if (refCountIt == slideshowDataRefCounts.end()) {
        throw std::runtime_error("Player component removed without having been added");
    }
    refCountIt->second -= 1;

    if (refCountIt->second == 0) {
        slideshowDataRefCounts.erase(refCountIt);
        const auto offsetIt = slideshowFrameMap.find(player.animation.get());
        if (offsetIt == slideshowFrameMap.end()) {
            throw std::runtime_error("Failed to find cached frame offset for animation");
        }

        slideshowFrameRangeAllocator.release(
            {offsetIt->second, static_cast<std::uint32_t>(player.animation->frameCount())});
        slideshowFrameMap.erase(offsetIt);
    }
}

void Animation2DSystem::updateSlideshowDescriptorSets() {
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
    vkUpdateDescriptorSets(
        renderer.vulkanState().device, std::size(setWrites), setWrites, 0, nullptr);
}

void Animation2DSystem::createNonSlideshow(com::Animation2D& anim,
                                           const com::Animation2DPlayer& player) {
    doNonSlideshowRemove(anim);
    VertexAnimation* data = doNonSlideshowCreate(player);
    data->useCount += 1;
    anim.systemHandle = data;
    anim.drawParams   = data->indexBuffer.getDrawParameters(); // indices overwritten in update()
}

Animation2DSystem::VertexAnimation* Animation2DSystem::doNonSlideshowCreate(
    const com::Animation2DPlayer& player) {
    auto it = vertexAnimationData.find(player.animation.get());
    if (it == vertexAnimationData.end()) {
        it = vertexAnimationData
                 .try_emplace(player.animation.get(), renderer.vulkanState(), *player.animation)
                 .first;
    }
    return &it->second;
}

void Animation2DSystem::doNonSlideshowRemove(const com::Animation2D& anim) {
    const auto it = std::find(vertexAnimations.begin(), vertexAnimations.end(), &anim);
    if (it != vertexAnimations.end()) {
        vertexAnimations.erase(it);
        tryFreeVertexData(*anim.player);
    }
}

void Animation2DSystem::tryFreeVertexData(const com::Animation2DPlayer& player) {
    const auto it = vertexAnimationData.find(player.animation.get());
    if (it != vertexAnimationData.end()) {
        it->second.useCount -= 1;
        if (it->second.useCount == 0) { vertexAnimationData.erase(it); }
    }
}

Animation2DSystem::VertexAnimation::VertexAnimation(rc::vk::VulkanState& vs,
                                                    const gfx::a2d::AnimationData& anim)
: useCount(0) {
    unsigned int actualShardCount = 0;
    for (unsigned int i = 0; i < anim.frameCount(); ++i) {
        actualShardCount += anim.getFrame(i).shards.size();
    }
    if (anim.frameCount() > 0) {
        frameToIndices.resize(anim.frameCount(), {});

        // allocate buffer
        const auto& lf                 = anim.getFrame(anim.frameCount() - 1);
        const std::uint32_t shardCount = actualShardCount; // lf.shardIndex + lf.shards.size();
        indexBuffer.create(vs, shardCount * 4, shardCount * 6);

        // populate buffer
        std::uint32_t vi = 0;
        std::uint32_t ii = 0;
        for (unsigned int i = 0; i < anim.frameCount(); ++i) {
            const auto& frame = anim.getFrame(i);

            // cache draw params
            frameToIndices[i].vertexStart = vi;
            frameToIndices[i].indexStart  = ii;
            frameToIndices[i].indexCount  = frame.shards.size() * 6;

            for (unsigned int j = 0; j < frame.shards.size(); ++j) {
                const auto& shard = frame.shards[j];

                // compute shard transform
                const glm::vec2 size(static_cast<float>(shard.source.width),
                                     static_cast<float>(shard.source.height));
                com::Transform2D transform;
                transform.setOrigin(anim.shardsAreCentered() ? size * 0.5f : glm::vec2{0.f, 0.f});
                transform.setRotation(shard.rotation);
                transform.setPosition({shard.offset.x, shard.offset.y});
                transform.setScale({shard.scale.x, shard.scale.y});
                const glm::mat4 tmat = transform.getMatrix();

                // set draw indices (CW winding)
                const auto ibase                 = ii + j * 6;
                const auto vbase                 = vi + j * 4;
                indexBuffer.indices()[ibase + 0] = vbase + 0;
                indexBuffer.indices()[ibase + 1] = vbase + 1;
                indexBuffer.indices()[ibase + 2] = vbase + 2;
                indexBuffer.indices()[ibase + 3] = vbase + 0;
                indexBuffer.indices()[ibase + 4] = vbase + 2;
                indexBuffer.indices()[ibase + 5] = vbase + 3;

                // set draw vertices
                // color
                for (unsigned int k = 0; k < 4; ++k) {
                    indexBuffer.vertices()[vbase + k].color =
                        glm::vec4{1.f, 1.f, 1.f, static_cast<float>(shard.alpha) / 255.f};
                }

                // pos
                glm::vec4 src{0.f, 0.f, 0.f, 1.f};
                indexBuffer.vertices()[vbase + 0].pos = tmat * src;
                src.x                                 = static_cast<float>(shard.source.width);
                indexBuffer.vertices()[vbase + 1].pos = tmat * src;
                src.y                                 = static_cast<float>(shard.source.height);
                indexBuffer.vertices()[vbase + 2].pos = tmat * src;
                src.x                                 = 0.f;
                indexBuffer.vertices()[vbase + 3].pos = tmat * src;
                if (anim.shardsAreCentered()) {
                    for (unsigned int k = 0; k < 4; ++k) {
                        indexBuffer.vertices()[vbase + k].pos -=
                            glm::vec3{size * glm::vec2{shard.scale.x, shard.scale.y} * 0.5f, 0.f};
                    }
                }

                // texture coord
                indexBuffer.vertices()[vbase + 0].texCoord = {shard.normalizedSource.left,
                                                              shard.normalizedSource.top};
                indexBuffer.vertices()[vbase + 1].texCoord = {shard.normalizedSource.left +
                                                                  shard.normalizedSource.width,
                                                              shard.normalizedSource.top};
                indexBuffer.vertices()[vbase + 2].texCoord = {
                    shard.normalizedSource.left + shard.normalizedSource.width,
                    shard.normalizedSource.top + shard.normalizedSource.height};
                indexBuffer.vertices()[vbase + 3].texCoord = {shard.normalizedSource.left,
                                                              shard.normalizedSource.top +
                                                                  shard.normalizedSource.height};
            }

            vi += frame.shards.size() * 4;
            ii += frameToIndices[i].indexCount;
        }
        indexBuffer.queueTransfer();
    }
}

} // namespace sys
} // namespace bl
