#ifndef BLIB_RENDER_DESCRIPTORS_INPUTATTACHMENTFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_INPUTATTACHMENTFACTORY_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Descriptor set factory that provides a single binding for an attachment set
 *
 * @ingroup Renderer
 */
class InputAttachmentFactory : public DescriptorSetFactory {
public:
    /**
     * @brief Creates the factory
     *
     * @param attachmentCount The number of attachments that will be bound
     */
    InputAttachmentFactory(std::uint32_t attachmentCount);

    /**
     * @brief Frees resources
     */
    virtual ~InputAttachmentFactory() = default;

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