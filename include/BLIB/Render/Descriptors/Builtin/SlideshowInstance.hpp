#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_SLIDESHOWINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_SLIDESHOWINSTANCE_HPP

#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Provides slideshow data for animations to render
 *
 * @ingroup Renderer
 */
class SlideshowInstance : public DescriptorSetInstance {
public:
    /**
     * @brief Create a new set instance
     *
     * @param engine Game engine instance
     * @param descriptorSetLayout Layout of the descriptor set
     */
    SlideshowInstance(engine::Engine& engine, VkDescriptorSetLayout descriptorSetLayout);

    /**
     * @brief Destroys the descriptor set instance
     */
    virtual ~SlideshowInstance() = default;

private:
    // TODO - data

    virtual void init(DescriptorComponentStorageCache& storageCache) override;
    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const override;
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void releaseObject(ecs::Entity entity, scene::Key key) override;
    virtual void handleFrameStart() override;
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
