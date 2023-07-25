#ifndef BLIB_RENDER_DESCRIPTORS_FADEEFFECTFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_FADEEFFECTFACTORY_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Descriptor set factory that provides the layout for the fade effect descriptor set
 *
 * @ingroup Renderer
 */
class FadeEffectFactory : public DescriptorSetFactory {
public:
    /**
     * @brief Frees resources
     */
    virtual ~FadeEffectFactory() = default;

private:
    virtual void init(engine::Engine& engine, Renderer& renderer) override;
    virtual std::unique_ptr<DescriptorSetInstance> createDescriptorSet() const override;
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif