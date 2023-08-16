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
, public event::Listener<ecs::event::ComponentAdded<com::Animation2DPlayer>>
, public event::Listener<ecs::event::ComponentRemoved<com::Animation2DPlayer>> {
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
        glm::vec2 texCoords[4];
        float opacity;
    };

    const VkDevice device;
    rc::Renderer& renderer;
    VkDescriptorSetLayout descriptorLayout;
    ecs::ComponentPool<com::Animation2DPlayer>* players;

    // slideshow data
    std::unordered_map<gfx::a2d::AnimationData*, std::uint32_t> slideshowFrameMap;
    std::uint32_t nextSlideshowPlayerIndex;                      // TODO - id allocator unbounded
    rc::buf::StaticSSBO<SlideshowFrame> slideshowFramesSSBO;     // all anim frames
    rc::buf::StaticSSBO<std::uint32_t> slideshowFrameOffsetSSBO; // playerIndex -> frame index
    rc::buf::DynamicSSBO<std::uint32_t> slideshowPlayerCurrentFrameSSBO; //     -> current frame
    rc::vk::PerFrame<rc::vk::DescriptorSet> slideshowDescriptorSets;
    std::uint8_t slideshowRefreshRequired;
    std::uint32_t lastSlideshowFrameUploaded;
    std::uint32_t lastSlideshowOffsetUploaded;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt) override;
    virtual void observe(const ecs::event::ComponentAdded<com::Animation2DPlayer>& event) override;
    virtual void observe(
        const ecs::event::ComponentRemoved<com::Animation2DPlayer>& event) override;

    void doSlideshowAdd(ecs::Entity playerEntity, com::Animation2DPlayer& player);
    void updateSlideshowDescrptorSets();
};

} // namespace sys
} // namespace bl

#endif
