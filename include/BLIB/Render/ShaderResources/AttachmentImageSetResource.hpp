#ifndef BLIB_RENDER_SHADERRESOURCES_ATTACHMENTIMAGESETRESOURCE_HPP
#define BLIB_RENDER_SHADERRESOURCES_ATTACHMENTIMAGESETRESOURCE_HPP

#include <BLIB/Render/ShaderResources/ShaderResource.hpp>
#include <BLIB/Render/Vulkan/AttachmentImageSet.hpp>
#include <array>

namespace bl
{
namespace rc
{
namespace sri
{
/**
 * @brief Shader resource for a list of attachment images
 *
 * @ingroup Renderer
 */
template<unsigned int AttachmentCount,
         std::array<vk::SemanticTextureFormat, AttachmentCount> Formats,
         std::array<VkImageUsageFlags, AttachmentCount> Usages>
class AttachmentImageSetResource : public sr::ShaderResource {
public:
    /**
     * @brief Creates the shader resource
     */
    AttachmentImageSetResource() = default;

    /**
     * @brief Destroys the shader resource
     */
    virtual ~AttachmentImageSetResource() = default;

private:
    vk::AttachmentImageSet images;
    unsigned int dirtyFrameCount;

    virtual void init(engine::Engine& engine) override;
    virtual void cleanup() override;
    virtual void performTransfer() override;
    virtual void copyFromSource() override;
    virtual bool dynamicDescriptorUpdateRequired() const override;
    virtual bool staticDescriptorUpdateRequired() const override;
};

} // namespace sri
} // namespace rc
} // namespace bl

#endif
