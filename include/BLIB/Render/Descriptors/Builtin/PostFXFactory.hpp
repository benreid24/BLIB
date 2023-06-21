#ifndef BLIB_RENDER_DESCRIPTORS_POSTFXFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_POSTFXFACTORY_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

namespace bl
{
namespace gfx
{
namespace ds
{
/**
 * @brief Descriptor set factory that provides the layout for the base PostFX class descriptor set
 *
 * @ingroup Renderer
 */
class PostFXFactory : public DescriptorSetFactory {
public:
    /**
     * @brief Frees resources
     */
    virtual ~PostFXFactory() = default;

private:
    virtual void init(engine::Engine& engine, Renderer& renderer) override;
    virtual std::unique_ptr<DescriptorSetInstance> createDescriptorSet() const override;
};

} // namespace ds
} // namespace gfx
} // namespace bl

#endif
