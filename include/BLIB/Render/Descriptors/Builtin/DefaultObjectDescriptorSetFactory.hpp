#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_DEFAULTOBJECTDESCRIPTORSETFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_DEFAULTOBJECTDESCRIPTORSETFACTORY_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

namespace bl
{
namespace render
{
namespace ds
{
class DefaultObjectDescriptorSetFactory : public DescriptorSetFactory {
public:
    virtual ~DefaultObjectDescriptorSetFactory() = default;

    virtual void init(engine::Engine& engine, Renderer& renderer) override;

    virtual std::unique_ptr<DescriptorSetInstance> createDescriptorSet() const override;

private:
    //
};

} // namespace ds
} // namespace render
} // namespace bl

#endif
