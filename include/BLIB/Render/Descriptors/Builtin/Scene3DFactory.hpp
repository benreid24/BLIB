#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_SCENE3DFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_SCENE3DFACTORY_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

namespace bl
{
namespace gfx
{
namespace vk
{
struct VulkanState;
}

namespace ds
{
/**
 * @brief Placeholder factory for scene descriptor sets. Factory itself only provides the descriptor
 *        set layout. It does not instantiate the instances
 *
 * @ingroup Renderer
 */
class Scene3DFactory : public DescriptorSetFactory {
public:
    /**
     * @brief Destroys the factory
     */
    virtual ~Scene3DFactory();

private:
    vk::VulkanState* vulkanState;

    virtual void init(engine::Engine&, Renderer& renderer) override;
    virtual std::unique_ptr<DescriptorSetInstance> createDescriptorSet() const override;
};

} // namespace ds
} // namespace gfx
} // namespace bl

#endif
