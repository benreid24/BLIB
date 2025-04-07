#ifndef BLIB_RENDER_DESCRIPTORS_COLORATTACHMENTFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_COLORATTACHMENTFACTORY_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Descriptor set factory that provides a single binding for a color attachment
 *
 * @ingroup Renderer
 */
class ColorAttachmentFactory : public DescriptorSetFactory {
public:
    /**
     * @brief Frees resources
     */
    virtual ~ColorAttachmentFactory() = default;

private:
    vk::VulkanState* vs;

    virtual void init(engine::Engine& engine, Renderer& renderer) override;
    virtual std::unique_ptr<DescriptorSetInstance> createDescriptorSet() const override;
    virtual std::type_index creates() const override;
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif