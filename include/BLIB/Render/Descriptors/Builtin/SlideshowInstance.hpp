#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_SLIDESHOWINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_SLIDESHOWINSTANCE_HPP

#include <BLIB/Components/Animation2DPlayer.hpp>
#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/Render/Buffers/DynamicSSBO.hpp>
#include <BLIB/Render/Buffers/StaticSSBO.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Vulkan/DescriptorSet.hpp>

namespace bl
{
namespace sys
{
class Animation2DSystem;
}

namespace rc
{
namespace dsi
{
/**
 * @brief Provides slideshow data for animations to render
 *
 * @ingroup Renderer
 */
class SlideshowInstance : public ds::DescriptorSetInstance {
public:
    /**
     * @brief Create a new set instance
     *
     * @param engine Game engine instance
     */
    SlideshowInstance(engine::Engine& engine);

    /**
     * @brief Destroys the descriptor set instance
     */
    virtual ~SlideshowInstance() = default;

private:
    sys::Animation2DSystem& animSystem;

    virtual void init(sr::ShaderResourceStore& storageCache) override;
    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const override;
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void releaseObject(ecs::Entity entity, scene::Key key) override;
    virtual void handleFrameStart() override;
};

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
