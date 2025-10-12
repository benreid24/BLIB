#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_GLOBALDATAINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_GLOBALDATAINSTANCE_HPP

#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Resources/GlobalDescriptors.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
/**
 * @brief Descriptor set instance that contains renderer global data such as texture and material
 *        pools
 *
 * @ingroup Renderer
 */
class GlobalDataInstance : public ds::DescriptorSetInstance {
public:
    /**
     * @brief Creates the descriptor set instance
     *
     * @param globals The renderer global data
     */
    GlobalDataInstance(res::GlobalDescriptors& globals);

    /**
     * @brief Destroys the descriptor set instance
     */
    virtual ~GlobalDataInstance() = default;

private:
    res::GlobalDescriptors& globals;

    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const override;
    virtual void releaseObject(ecs::Entity entity, scene::Key objectKey) override;
    virtual void init(sr::ShaderResourceStore& globalShaderResources,
                      sr::ShaderResourceStore& sceneShaderResources,
                      sr::ShaderResourceStore& observerShaderResources) override;
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void updateDescriptors() override;
};

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
