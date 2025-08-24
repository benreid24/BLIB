#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_SCENE3DFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_SCENE3DFACTORY_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanState;
}

namespace dsi
{
/**
 * @brief Placeholder factory for scene descriptor sets. Factory itself only provides the descriptor
 *        set layout. It does not instantiate the instances
 *
 * @ingroup Renderer
 */
class Scene3DFactory : public ds::DescriptorSetFactory {
public:
    /**
     * @brief Destroys the factory
     */
    virtual ~Scene3DFactory();

private:
    Renderer* renderer;
    vk::VulkanState* vulkanState;

    virtual void init(engine::Engine&, Renderer& renderer) override;
    virtual std::unique_ptr<ds::DescriptorSetInstance> createDescriptorSet() const override;
    virtual std::type_index creates() const override;
};

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
