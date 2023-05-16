#ifndef BLIB_RENDER_DESCRIPTORS_BUILTINT_TEXTURESDESCRIPTORSETFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTINT_TEXTURESDESCRIPTORSETFACTORY_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

namespace bl
{
namespace render
{
namespace res
{
class TexturePool;
}

namespace ds
{
/**
 * @brief Descriptor set factory to use when a pipeline needs the bindless texture atlas
 *
 * @ingroup Renderer
 */
class TexturesDescriptorSetFactory : public DescriptorSetFactory {
public:
    /**
     * @brief Creates the factory
     */
    TexturesDescriptorSetFactory() = default;

    /**
     * @brief Destroys the factory
     */
    virtual ~TexturesDescriptorSetFactory() = default;

private:
    res::TexturePool* texturePool;

    virtual void init(engine::Engine& engine, Renderer& renderer) override;
    virtual std::unique_ptr<DescriptorSetInstance> createDescriptorSet() const override;
};

} // namespace ds
} // namespace render
} // namespace bl

#endif
