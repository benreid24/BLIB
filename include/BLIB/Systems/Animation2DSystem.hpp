#ifndef BLIB_SYSTEMS_ANIMATION2DPLAYER_HPP
#define BLIB_SYSTEMS_ANIMATION2DPLAYER_HPP

#include <BLIB/Components/Animation2D.hpp>
#include <BLIB/Components/Animation2DPlayer.hpp>
#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Render/Buffers/BufferDoubleHostVisibleSourced.hpp>
#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Buffers/StaticSSBO.hpp>
#include <BLIB/Render/Vulkan/DescriptorSet.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Signals/Listener.hpp>
#include <BLIB/Util/IdAllocatorUnbounded.hpp>
#include <BLIB/Util/RangeAllocatorUnbounded.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
class Renderer;
}

namespace sys
{
/**
 * @brief Systems that updates and advances all 2d animations. Also manages GPU local buffers
 *        containing animation data
 *
 * @ingroup Systems
 */
class Animation2DSystem
: public engine::System
, public sig::Listener<ecs::event::ComponentAdded<com::Animation2DPlayer>,
                       ecs::event::ComponentRemoved<com::Animation2DPlayer>,
                       ecs::event::ComponentRemoved<com::Animation2D>> {
public:
    /**
     * @brief Creates the animation system
     */
    Animation2DSystem(rc::Renderer& renderer);

    /**
     * @brief Destroys the animation system
     */
    virtual ~Animation2DSystem();

    /**
     * @brief Binds the animation descriptor set for slideshow animations
     *
     * @param commandBuffer The command buffer to issue bind commands into
     * @param layout The current pipeline layout
     * @param setIndex The index to bind to
     */
    void bindSlideshowSet(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                          std::uint32_t setIndex);

private:
    struct SlideshowFrame {
        alignas(16) glm::vec2 texCoords[4];
        alignas(16) float opacity;
    };

    struct VertexAnimation {
        struct DrawIndices {
            std::uint32_t vertexStart;
            std::uint32_t indexStart;
            std::uint32_t indexCount;
        };

        rc::buf::IndexBuffer indexBuffer;
        std::vector<DrawIndices> frameToIndices;
        unsigned int useCount;

        VertexAnimation(rc::Renderer& renderer, const gfx::a2d::AnimationData& anim);
    };

    rc::Renderer& renderer;
    VkDescriptorSetLayout descriptorLayout;
    ecs::ComponentPool<com::Animation2DPlayer>* players;
    ecs::ComponentPool<com::Animation2D>* vertexPool;
    resource::Ref<gfx::a2d::AnimationData> errorAnim;

    // slideshow data
    std::unordered_map<const gfx::a2d::AnimationData*, std::uint32_t> slideshowFrameMap;
    std::unordered_map<const gfx::a2d::AnimationData*, std::uint32_t> slideshowDataRefCounts;
    util::IdAllocatorUnbounded<std::uint32_t> slideshowPlayerIds;
    util::RangeAllocatorUnbounded<std::uint32_t> slideshowFrameRangeAllocator;
    rc::buf::StaticSSBO<SlideshowFrame> slideshowFramesSSBO;     // all anim frames
    rc::buf::StaticSSBO<std::uint32_t> slideshowFrameOffsetSSBO; // playerIndex -> frame index
    rc::buf::StaticSSBO<std::uint32_t> slideshowTextureSSBO;     // playerIndex -> texture id
    rc::buf::BufferDoubleHostVisibleSourced<std::uint32_t> slideshowPlayerCurrentFrameSSBO; //     -> current frame
    rc::vk::PerFrame<rc::vk::DescriptorSet> slideshowDescriptorSets;
    std::uint8_t slideshowRefreshRequired;
    std::uint8_t slideshowLastFrameUpdated; // renderer frame index to prevent multiple updates

    // non-slideshow data
    std::unordered_map<const gfx::a2d::AnimationData*, VertexAnimation> vertexAnimationData;
    std::vector<com::Animation2D*> vertexAnimations;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt, float, float, float) override;
    virtual void process(const ecs::event::ComponentAdded<com::Animation2DPlayer>& event) override;
    virtual void process(
        const ecs::event::ComponentRemoved<com::Animation2DPlayer>& event) override;
    virtual void process(const ecs::event::ComponentRemoved<com::Animation2D>& event) override;
    virtual void earlyCleanup() override;

    // slideshow methods
    void doSlideshowAdd(com::Animation2DPlayer& player);
    void doSlideshowFree(const com::Animation2DPlayer& player);
    void ensureSlideshowDescriptorsUpdated();
    void updateSlideshowDescriptorSets();

    // non-slideshow methods
    void createNonSlideshow(com::Animation2D& anim, const com::Animation2DPlayer& player);
    VertexAnimation* doNonSlideshowCreate(const com::Animation2DPlayer& player);
    void doNonSlideshowRemove(const com::Animation2D& anim);
    void tryFreeVertexData(const com::Animation2DPlayer& player);

    friend class rc::Renderer;
    friend struct com::Animation2D;
};

} // namespace sys
} // namespace bl

#endif
