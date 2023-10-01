#ifndef BLIB_SYSTEMS_ANIMATION2DPLAYER_HPP
#define BLIB_SYSTEMS_ANIMATION2DPLAYER_HPP

#include <BLIB/Components/Animation2DPlayer.hpp>
#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Render/Buffers/DynamicSSBO.hpp>
#include <BLIB/Render/Buffers/StaticSSBO.hpp>
#include <BLIB/Render/Vulkan/DescriptorSet.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Util/IdAllocatorUnbounded.hpp>
#include <BLIB/Util/RangeAllocatorUnbounded.hpp>
#include <glm/glm.hpp>

namespace bl
{
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
, public event::Listener<ecs::event::ComponentAdded<com::Animation2DPlayer>,
                         ecs::event::ComponentRemoved<com::Animation2DPlayer>> {
public:
    /**
     * @brief Creates the animation system
     */
    Animation2DSystem(rc::Renderer& renderer);

    /**
     * @brief Destroys the animation system
     */
    virtual ~Animation2DSystem() = default;

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

    struct UpdateRange {
        static constexpr std::uint32_t InvalidStart = std::numeric_limits<std::uint32_t>::max();

        std::uint32_t start;
        std::uint32_t size;

        UpdateRange()
        : start(InvalidStart)
        , size(0) {}

        void reset() {
            start = InvalidStart;
            size  = 0;
        }

        bool needsUpload() const { return size != 0; }

        void addRange(std::uint32_t i, std::uint32_t len) {
            const std::uint32_t prevEnd = start == InvalidStart ? 0 : start + len;
            start                       = std::min(start, i);
            size                        = std::max(prevEnd, i + len);
            size -= start;
        }
    };

    rc::Renderer& renderer;
    VkDescriptorSetLayout descriptorLayout;
    ecs::ComponentPool<com::Animation2DPlayer>* players;

    // slideshow data
    std::unordered_map<const gfx::a2d::AnimationData*, std::uint32_t> slideshowFrameMap;
    std::unordered_map<const gfx::a2d::AnimationData*, std::uint32_t> slideshowDataRefCounts;
    util::IdAllocatorUnbounded<std::uint32_t> slideshowPlayerIds;
    util::RangeAllocatorUnbounded<std::uint32_t> slideshowFrameRangeAllocator;
    rc::buf::StaticSSBO<SlideshowFrame> slideshowFramesSSBO;     // all anim frames
    rc::buf::StaticSSBO<std::uint32_t> slideshowFrameOffsetSSBO; // playerIndex -> frame index
    rc::buf::DynamicSSBO<std::uint32_t> slideshowPlayerCurrentFrameSSBO; //     -> current frame
    rc::vk::PerFrame<rc::vk::DescriptorSet> slideshowDescriptorSets;
    std::uint8_t slideshowRefreshRequired;
    std::uint8_t slideshowLastFrameUpdated; // renderer frame index to prevent multiple updates
    UpdateRange slideshowFrameUploadRange;
    UpdateRange slideshowOffsetUploadRange;

    void cleanup();
    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt) override;
    virtual void observe(const ecs::event::ComponentAdded<com::Animation2DPlayer>& event) override;
    virtual void observe(
        const ecs::event::ComponentRemoved<com::Animation2DPlayer>& event) override;

    void doSlideshowAdd(ecs::Entity playerEntity, com::Animation2DPlayer& player);
    void doSlideshowFree(ecs::Entity playerEntity, const com::Animation2DPlayer& player);
    void ensureSlideshowDescriptorsUpdated();
    void updateSlideshowDescriptorSets();

    friend class rc::Renderer;
};

} // namespace sys
} // namespace bl

#endif
