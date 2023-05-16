#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_TEXTURESDESCRIPTORSETINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_TEXTURESDESCRIPTORSETINSTANCE_HPP

#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Resources/TexturePool.hpp>

namespace bl
{
namespace render
{
namespace ds
{
/**
 * @brief Descriptor set instance that binds the bindless texture pool descriptor set
 *
 * @ingroup Renderer
 */
class TexturesDescriptorSetInstance : public DescriptorSetInstance {
public:
    /**
     * @brief Creates the descriptor set instance
     *
     * @param texturePool Texture pool to bind
     */
    TexturesDescriptorSetInstance(res::TexturePool& texturePool);

    /**
     * @brief Destroys the descriptor set instance
     */
    virtual ~TexturesDescriptorSetInstance() = default;

private:
    res::TexturePool& texturePool;

    virtual void bindForPipeline(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                                 std::uint32_t observerIndex,
                                 std::uint32_t setIndex) const override;
    virtual void bindForObject(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                               std::uint32_t setIndex, std::uint32_t objectId) const override;
    virtual void releaseObject(std::uint32_t sceneId, ecs::Entity entity) override;
    virtual void doInit(std::uint32_t maxStaticObjects, std::uint32_t maxDynamicObjects) override;
    virtual bool doAllocateObject(std::uint32_t sceneId, ecs::Entity entity,
                                  UpdateSpeed updateSpeed) override;
    virtual void beginSync(bool staticObjectsChanged) override;
};

} // namespace ds
} // namespace render
} // namespace bl

#endif
