#ifndef BLIB_RENDER_OVERLAYS_DRAWABLES_IMAGE_HPP
#define BLIB_RENDER_OVERLAYS_DRAWABLES_IMAGE_HPP

#include <BLIB/Render/Overlays/Drawable.hpp>
#include <BLIB/Render/Primitives/IndexBuffer.hpp>
#include <BLIB/Render/Util/PerFrame.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentSet.hpp>
#include <array>
#include <glad/vulkan.h>

namespace bl
{
namespace render
{
class Renderer;

namespace overlay
{
/**
 * @brief Utility class to render images from StandardImageBuffers onto a color attachment. Used to
 *        transfer rendered scenes to the swap chain image for each observer.
 *
 * @ingroup Renderer
 */
class Image : public Drawable {
public:
    Image();

    virtual ~Image();

    void setImage(Renderer& renderer, VkImageView imageView);

    // TODO - interface to easily allow textures

    void cleanup();

    void usePipeline(std::uint32_t pipelineId);

    virtual void doRender(OverlayRenderContext& context) override;

    // TODO - somehow allow rendering to less than full viewport

    static std::array<VkDescriptorSetLayoutBinding, 1> DescriptorLayoutBindings();

private:
    Renderer* renderer;
    VkSampler sampler;               // get from standard image buffer
    VkDescriptorPool descriptorPool; // TODO - move this somewhere shared or into a pool allocator
    VkDescriptorSet descriptorSet;

    IndexBuffer indexBuffer;
    Pipeline* pipeline;
};

} // namespace overlay
} // namespace render
} // namespace bl

#endif
