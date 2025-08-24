#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_SLIDESHOWFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_SLIDESHOWFACTORY_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
/**
 * @brief Descriptor set factory for slideshow descriptor sets
 *
 * @ingroup Renderer
 */
class SlideshowFactory : public ds::DescriptorSetFactory {
public:
    /**
     * @brief Destroys the factory
     */
    virtual ~SlideshowFactory() = default;

private:
    engine::Engine* engine;

    virtual void init(engine::Engine& engine, Renderer& renderer) override;
    virtual std::unique_ptr<ds::DescriptorSetInstance> createDescriptorSet() const override;
    virtual std::type_index creates() const override;
};

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
