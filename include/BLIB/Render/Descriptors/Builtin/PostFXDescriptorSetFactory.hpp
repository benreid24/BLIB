#ifndef BLIB_RENDER_DESCRIPTORS_STATICDESCRIPTORSETFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_STATICDESCRIPTORSETFACTORY_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

namespace bl
{
namespace render
{
namespace ds
{
/**
 * @brief Descriptor set factory that provides the layout for the base PostFX class descriptor set
 *
 * @ingroup Renderer
 */
class PostFXDescriptorSetFactory : public DescriptorSetFactory {
public:
    /**
     * @brief Creates a new factory
     */
    PostFXDescriptorSetFactory();

    /**
     * @brief Frees resources
     */
    virtual ~PostFXDescriptorSetFactory();

private:
    VkDevice device;

    virtual void init(engine::Engine& engine, Renderer& renderer) override;
    virtual std::unique_ptr<DescriptorSetInstance> createDescriptorSet() const override;
};

} // namespace ds
} // namespace render
} // namespace bl

#endif
