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

resource::Ref<gfx::a2d::AnimationData> getErrorPlaceholder() {
    using ImageManager                = resource::ResourceManager<sf::Image>;
    using AnimationManager            = resource::ResourceManager<gfx::a2d::AnimationData>;
    constexpr const char* Spritesheet = "blib.animation.error_spritesheet";
    constexpr const char* Path        = "blib.animation.error_animation";

    if (!AnimationManager::available(Path)) {
        // Create spritesheet
        if (!ImageManager::available(Spritesheet)) {
            sf::Image img;
            img.resize({202, 100}, sf::Color::Transparent);

            const auto fillSquare = [&img](unsigned int baseX, sf::Color main, sf::Color alt) {
                constexpr unsigned int BoxSize = 4;
                for (unsigned int x = 0; x < 100; ++x) {
                    for (unsigned int y = 0; y < 100; ++y) {
                        const unsigned int xi = x / BoxSize;
                        const unsigned int yi = y / BoxSize;
                        const auto& col       = (xi % 2) == (yi % 2) ? main : alt;
                        img.setPixel({x + baseX, y}, col);
                    }
                }
            };

            const sf::Color c1(230, 66, 245);
            const sf::Color c2(255, 254, 196);
            fillSquare(0, c1, c2);
            fillSquare(102, c2, c1);
            ImageManager::put(Spritesheet, img);
        }

        std::vector<gfx::a2d::AnimationData::Frame> frames;
        frames.resize(2);
        for (unsigned int i = 0; i < 2; ++i) {
            auto& frame    = frames[i];
            frame.length   = 0.75f;
            auto& shard    = frame.shards.emplace_back();
            shard.source   = sf::IntRect{{i == 0 ? 0 : 102, 0}, {100, 100}};
            shard.alpha    = 255;
            shard.rotation = 0.f;
            shard.offset   = {0.f, 0.f};
            shard.scale    = {1.f, 1.f};
        }

        gfx::a2d::AnimationData anim;
        anim.debugInitialize(Spritesheet, std::move(frames), true, false);
        return AnimationManager::put(Path, anim);
    }

    return AnimationManager::load(Path);
}
} // namespace

Animation2DSystem::Animation2DSystem(rc::Renderer& renderer)
: renderer(renderer)
, players(nullptr)
, slideshowFrameRangeAllocator(InitialSlideshowFrameCapacity)
, slideshowRefreshRequired(rc::cfg::Limits::MaxConcurrentFrames)
, slideshowLastFrameUpdated(255) {
    slideshowDescriptorSets.init(renderer.vulkanState(),
                                 [&renderer](rc::vk::DescriptorSet& set) { set.init(renderer); });
}

Animation2DSystem::~Animation2DSystem() {}

void Animation2DSystem::earlyCleanup() {
    unsubscribe();
    slideshowDescriptorSets.cleanup([](rc::vk::DescriptorSet& ds) { ds.release(); });
    slideshowFramesSSBO.destroy();
    slideshowFrameOffsetSSBO.destroy();
    slideshowTextureSSBO.destroy();
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
                           .getOrCreateFactory<rc::dsi::SlideshowFactory>()
                           ->getDescriptorLayout();

    players    = &engine.ecs().getAllComponents<com::Animation2DPlayer>();
    vertexPool = &engine.ecs().getAllComponents<com::Animation2D>();

    slideshowFramesSSBO.create(renderer, InitialSlideshowFrameCapacity);
    slideshowFrameOffsetSSBO.create(renderer, 32);
    slideshowTextureSSBO.create(renderer, 32);
    slideshowPlayerCurrentFrameSSBO.create(renderer, 32);
    slideshowPlayerCurrentFrameSSBO.transferEveryFrame();

    subscribe(engine.ecs().getSignalChannel());
}

void Animation2DSystem::update(std::mutex&, float dt, float, float, float) {
    // play animations
    players->forEach([dt](ecs::Entity, com::Animation2DPlayer& player) { player.update(dt); });

    // perform slideshow uploads
    slideshowPlayerCurrentFrameSSBO.markFullDirty(); // always upload all play indices

    // sync vertex animation draw parameters
    vertexPool->forEach([](ecs::Entity, com::Animation2D& anim) {
        if (anim.systemHandle && anim.player && anim.getSceneRef().object) {
            const VertexAnimation& data = *static_cast<VertexAnimation*>(anim.systemHandle);
            const auto& frame           = data.frameToIndices[anim.player->currentFrame];
            anim.updateDrawParams(frame.indexStart, frame.indexCount);
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

void Animation2DSystem::process(const ecs::event::ComponentAdded<com::Animation2DPlayer>& event) {
    if (event.component.animation->frameCount() == 0) {
        auto& component     = const_cast<com::Animation2DPlayer&>(event.component);
        component.animation = getErrorPlaceholder();
    }

    if (event.component.forSlideshow) {
        if (!event.component.animation->isSlideshow()) {
            BL_LOG_ERROR << "Non-slideshow animation being used as slideshow";
        }
        doSlideshowAdd(event.component);
    }
    else { doNonSlideshowCreate(event.component); }
}

void Animation2DSystem::process(const ecs::event::ComponentRemoved<com::Animation2DPlayer>& event) {
    if (event.component.forSlideshow) { doSlideshowFree(event.component); }
    else { tryFreeVertexData(event.component); }
}

void Animation2DSystem::process(const ecs::event::ComponentRemoved<com::Animation2D>& event) {
    doNonSlideshowRemove(event.component);
}

void Animation2DSystem::doSlideshowAdd(com::Animation2DPlayer& player) {
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
        players->forEach([this](ecs::Entity, com::Animation2DPlayer& p) {
            if (p.framePayload.valid()) {
                slideshowPlayerCurrentFrameSSBO.assignRef(p.framePayload, p.playerIndex);
            }
        });
    }
    else { slideshowPlayerCurrentFrameSSBO.assignRef(player.framePayload, player.playerIndex); }
    slideshowFrameOffsetSSBO[index]        = offset;
    slideshowPlayerCurrentFrameSSBO[index] = player.currentFrame;
    slideshowFrameOffsetSSBO.markDirty(index, 1);

    // fetch texture to convert texCoords and set texture id
    rc::res::TextureRef texture = renderer.texturePool().getOrLoadTexture(
        player.animation->resolvedSpritesheet(),
        {.format  = rc::vk::CommonTextureFormats::SRGBA32Bit,
         .sampler = rc::vk::SamplerOptions::Type::NoFilterEdgeClamped}); // TODO - parameterize?
    player.texture = texture;
    slideshowTextureSSBO.ensureSize(index + 1);
    slideshowTextureSSBO[index] = texture.id();
    slideshowTextureSSBO.markDirty(index, 1);

    // add frames to SSBO if required
    if (uploadFrames) {
        slideshowFrameMap[player.animation.get()] = offset;
        slideshowFramesSSBO.ensureSize(offset + player.animation->frameCount());
        for (std::size_t i = 0; i < player.animation->frameCount(); ++i) {
            const auto& src          = player.animation->getFrame(i).shards.front();
            auto& frame              = slideshowFramesSSBO[offset + i];
            const sf::FloatRect& tex = src.normalizedSource;
            frame.opacity            = static_cast<float>(src.alpha) / 255.f;
            frame.texCoords[0]       = texture->convertCoord({tex.position.x, tex.position.y});
            frame.texCoords[1] =
                texture->convertCoord({tex.position.x + tex.size.x, tex.position.y});
            frame.texCoords[2] =
                texture->convertCoord({tex.position.x + tex.size.x, tex.position.y + tex.size.y});
            frame.texCoords[3] =
                texture->convertCoord({tex.position.x, tex.position.y + tex.size.y});
        }
        slideshowFramesSSBO.markDirty(offset, player.animation->frameCount());
    }
    slideshowDataRefCounts[player.animation.get()] += 1;

    // mark that descriptors need to be reset
    slideshowRefreshRequired = rc::cfg::Limits::MaxConcurrentFrames;
}

void Animation2DSystem::doSlideshowFree(const com::Animation2DPlayer& player) {
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
    VkWriteDescriptorSet setWrites[4]{};

    // (re)allocate the descriptor sets
    slideshowDescriptorSets.current().allocate(descriptorLayout);

    // frame offset (binding 0)
    VkDescriptorBufferInfo frameOffsetWrite{};
    frameOffsetWrite.buffer = slideshowFrameOffsetSSBO.getCurrentFrameBuffer().getBuffer();
    frameOffsetWrite.offset = 0;
    frameOffsetWrite.range  = slideshowFrameOffsetSSBO.getTotalAlignedSize();

    setWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[0].descriptorCount = 1;
    setWrites[0].dstBinding      = 0;
    setWrites[0].dstArrayElement = 0;
    setWrites[0].dstSet          = slideshowDescriptorSets.current().getSet();
    setWrites[0].pBufferInfo     = &frameOffsetWrite;
    setWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    // texture id (binding 1)
    VkDescriptorBufferInfo textureIdWrite{};
    textureIdWrite.buffer = slideshowTextureSSBO.getCurrentFrameBuffer().getBuffer();
    textureIdWrite.offset = 0;
    textureIdWrite.range  = slideshowTextureSSBO.getTotalAlignedSize();

    setWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[1].descriptorCount = 1;
    setWrites[1].dstBinding      = 1;
    setWrites[1].dstArrayElement = 0;
    setWrites[1].dstSet          = slideshowDescriptorSets.current().getSet();
    setWrites[1].pBufferInfo     = &textureIdWrite;
    setWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    // current frames (binding 2)
    VkDescriptorBufferInfo currentFrameWrite{};
    currentFrameWrite.buffer = slideshowPlayerCurrentFrameSSBO.getCurrentFrameRawBuffer();
    currentFrameWrite.offset = 0;
    currentFrameWrite.range  = slideshowPlayerCurrentFrameSSBO.getTotalAlignedSize();

    setWrites[2].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[2].descriptorCount = 1;
    setWrites[2].dstBinding      = 2;
    setWrites[2].dstArrayElement = 0;
    setWrites[2].dstSet          = slideshowDescriptorSets.current().getSet();
    setWrites[2].pBufferInfo     = &currentFrameWrite;
    setWrites[2].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    // frame data (binding 3)
    VkDescriptorBufferInfo frameDataWrite{};
    frameDataWrite.buffer = slideshowFramesSSBO.getCurrentFrameBuffer().getBuffer();
    frameDataWrite.offset = 0;
    frameDataWrite.range  = slideshowFramesSSBO.getTotalAlignedSize();

    setWrites[3].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrites[3].descriptorCount = 1;
    setWrites[3].dstBinding      = 3;
    setWrites[3].dstArrayElement = 0;
    setWrites[3].dstSet          = slideshowDescriptorSets.current().getSet();
    setWrites[3].pBufferInfo     = &frameDataWrite;
    setWrites[3].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    // perform write
    vkUpdateDescriptorSets(
        renderer.vulkanState().getDevice(), std::size(setWrites), setWrites, 0, nullptr);
}

void Animation2DSystem::createNonSlideshow(com::Animation2D& anim,
                                           const com::Animation2DPlayer& player) {
    if (player.animation->frameCount() == 0) {
        const_cast<com::Animation2DPlayer&>(player).animation = getErrorPlaceholder();
    }
    doNonSlideshowRemove(anim);
    VertexAnimation* data = doNonSlideshowCreate(player);
    data->useCount += 1;
    anim.systemHandle = data;
    anim.initDrawParams(data->indexBuffer.getDrawParameters()); // indices overwritten in update()
}

Animation2DSystem::VertexAnimation* Animation2DSystem::doNonSlideshowCreate(
    const com::Animation2DPlayer& player) {
    auto it = vertexAnimationData.find(player.animation.get());
    if (it == vertexAnimationData.end()) {
        it = vertexAnimationData.try_emplace(player.animation.get(), renderer, *player.animation)
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

Animation2DSystem::VertexAnimation::VertexAnimation(rc::Renderer& renderer,
                                                    const gfx::a2d::AnimationData& anim)
: useCount(0) {
    unsigned int shardCount = 0;
    for (unsigned int i = 0; i < anim.frameCount(); ++i) {
        shardCount += anim.getFrame(i).shards.size();
    }
    if (anim.frameCount() > 0) {
        frameToIndices.resize(anim.frameCount(), {});

        // allocate buffer
        indexBuffer.create(renderer, shardCount * 4, shardCount * 6);

        // fetch texture to convert texCoords
        rc::res::TextureRef texture =
            renderer.texturePool().getOrLoadTexture(anim.resolvedSpritesheet());

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
                const glm::vec2 size(static_cast<float>(shard.source.size.x),
                                     static_cast<float>(shard.source.size.y));
                com::Transform2D transform;
                transform.setOrigin(anim.shardsAreCentered() ? size * 0.5f : glm::vec2{0.f, 0.f});
                transform.setRotation(shard.rotation);
                transform.setPosition({shard.offset.x, shard.offset.y});
                transform.setScale({shard.scale.x, shard.scale.y});
                const glm::mat4 tmat = transform.getLocalTransform();

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
                src.x                                 = static_cast<float>(shard.source.size.x);
                indexBuffer.vertices()[vbase + 1].pos = tmat * src;
                src.y                                 = static_cast<float>(shard.source.size.y);
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
                indexBuffer.vertices()[vbase + 0].texCoord = {shard.normalizedSource.position.x,
                                                              shard.normalizedSource.position.y};
                indexBuffer.vertices()[vbase + 1].texCoord = {shard.normalizedSource.position.x +
                                                                  shard.normalizedSource.size.x,
                                                              shard.normalizedSource.position.y};
                indexBuffer.vertices()[vbase + 2].texCoord = {
                    shard.normalizedSource.position.x + shard.normalizedSource.size.x,
                    shard.normalizedSource.position.y + shard.normalizedSource.size.y};
                indexBuffer.vertices()[vbase + 3].texCoord = {shard.normalizedSource.position.x,
                                                              shard.normalizedSource.position.y +
                                                                  shard.normalizedSource.size.y};
                for (unsigned int i = 0; i < 4; ++i) {
                    indexBuffer.vertices()[vbase + i].texCoord =
                        texture->convertCoord(indexBuffer.vertices()[vbase + i].texCoord);
                }
            }

            vi += frame.shards.size() * 4;
            ii += frameToIndices[i].indexCount;
        }
        indexBuffer.queueTransfer();
    }
}

} // namespace sys
} // namespace bl
