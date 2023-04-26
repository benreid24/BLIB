#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_COMMONSCENEDESCRIPTORSETFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_COMMONSCENEDESCRIPTORSETFACTORY_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

namespace bl
{
namespace render
{
struct VulkanState;

namespace ds
{
/**
 * @brief Placeholder factory for scene descriptor sets. Factory itself only provides the descriptor
 *        set layout. It does not instantiate the instances
 *
 * @ingroup Renderer
 */
class CommonSceneDescriptorSetFactory : public DescriptorSetFactory {
public:
    /**
     * @brief Destroys the factory
     */
    virtual ~CommonSceneDescriptorSetFactory();

private:
    VulkanState* vulkanState;

    virtual void init(engine::Engine&, Renderer& renderer) override;
    virtual std::unique_ptr<DescriptorSetInstance> createDescriptorSet() const override;
};

} // namespace ds
} // namespace render
} // namespace bl

#endif
