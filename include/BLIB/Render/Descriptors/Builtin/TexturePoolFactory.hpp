#ifndef BLIB_RENDER_DESCRIPTORS_BUILTINT_TEXTUREPOOLFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTINT_TEXTUREPOOLFACTORY_HPP

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
class TexturePoolFactory : public DescriptorSetFactory {
public:
    /**
     * @brief Creates the factory
     */
    TexturePoolFactory() = default;

    /**
     * @brief Destroys the factory
     */
    virtual ~TexturePoolFactory() = default;

private:
    res::TexturePool* texturePool;

    virtual void init(engine::Engine& engine, Renderer& renderer) override;
    virtual std::unique_ptr<DescriptorSetInstance> createDescriptorSet() const override;
};

} // namespace ds
} // namespace render
} // namespace bl

#endif