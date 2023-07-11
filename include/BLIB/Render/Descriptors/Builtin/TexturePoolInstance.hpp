#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_TEXTURESDESCRIPTORSETINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_TEXTURESDESCRIPTORSETINSTANCE_HPP

#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Resources/TexturePool.hpp>

namespace bl
{
namespace gfx
{
namespace ds
{
/**
 * @brief Descriptor set instance that binds the bindless texture pool descriptor set
 *
 * @ingroup Renderer
 */
class TexturePoolInstance : public DescriptorSetInstance {
public:
    /**
     * @brief Creates the descriptor set instance
     *
     * @param texturePool Texture pool to bind
     */
    TexturePoolInstance(res::TexturePool& texturePool);

    /**
     * @brief Destroys the descriptor set instance
     */
    virtual ~TexturePoolInstance() = default;

private:
    res::TexturePool& texturePool;

    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const override;
    virtual void releaseObject(ecs::Entity entity, scene::Key objectKey) override;
    virtual void init(DescriptorComponentStorageCache& storageCache) override;
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void handleFrameStart() override;
};

} // namespace ds
} // namespace gfx
} // namespace bl

#endif
