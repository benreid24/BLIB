#include <BLIB/Render/Overlays/Drawables/Image.hpp>

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
namespace overlay
{
namespace
{
const prim::Vertex vertices[4] = {prim::Vertex({-1.f, -1.f, 0.f}, {0.f, 0.f}),
                                  prim::Vertex({1.f, -1.f, 0.f}, {1.f, 0.f}),
                                  prim::Vertex({1.f, 1.f, 0.f}, {1.f, 1.f}),
                                  prim::Vertex({-1.f, 1.f, 0.f}, {0.f, 1.f})};

const std::uint32_t indices[6] = {0, 1, 3, 1, 2, 3};
} // namespace

Image::Image()
: renderer(nullptr)
, sampler(nullptr) {}

Image::~Image() { cleanup(); }

void Image::setImage(Renderer& r, VkImageView imageView) {
    renderer        = &r;
    VulkanState& vs = renderer->vulkanState();

    // check if first time init
    if (sampler == nullptr) {
        // create sampler
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType            = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter        = VK_FILTER_NEAREST;
        samplerInfo.minFilter        = VK_FILTER_NEAREST;
        samplerInfo.addressModeU     = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.addressModeV     = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.addressModeW     = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy    = vs.physicalDeviceProperties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor      = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE; // dont need extent then
        samplerInfo.compareEnable           = VK_FALSE;
        samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias              = 0.0f;
        samplerInfo.minLod                  = 0.0f;
        samplerInfo.maxLod                  = 0.0f;
        if (vkCreateSampler(vs.device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }

        // allocate descriptor set
        const auto* createInfo = &vs.descriptorSetLayouts.imageOverlay.createInfo;
        setHandle              = vs.descriptorPool.allocate(
            &createInfo, &vs.descriptorSetLayouts.imageOverlay.layout, &descriptorSet, 1);

        // create index buffer
        indexBuffer.create(vs, 4, 6);
        indexBuffer.indices().write(indices, 0, 6);
        indexBuffer.vertices().write(vertices, 0, 4);
        indexBuffer.sendToGPU();

        // fetch initial pipeline
        usePipeline(Config::PipelineIds::ImageOverlay);
    }

    // write descriptor sets
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView   = imageView;
    imageInfo.sampler     = sampler;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet          = descriptorSet;
    descriptorWrite.dstBinding      = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo      = &imageInfo;
    vkUpdateDescriptorSets(vs.device, 1, &descriptorWrite, 0, nullptr);
}

void Image::cleanup() {
    if (sampler != nullptr) {
        VulkanState& vs        = renderer->vulkanState();
        const auto* createInfo = &vs.descriptorSetLayouts.imageOverlay.createInfo;

        vs.descriptorPool.release(setHandle,
                                  &createInfo,
                                  &vs.descriptorSetLayouts.imageOverlay.layout,
                                  &descriptorSet,
                                  1);
        vkDestroySampler(vs.device, sampler, nullptr);
        indexBuffer.destroy();
        sampler = nullptr;
    }
}

void Image::usePipeline(std::uint32_t pid) {
    pipeline = &renderer->pipelineCache().getPipeline(pid);
}

void Image::doRender(OverlayRenderContext& ctx) {
    if (!sampler) return;

    ctx.bindPipeline(pipeline->rawPipeline());
    vkCmdBindDescriptorSets(ctx.commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline->pipelineLayout(),
                            0,
                            1,
                            &descriptorSet,
                            0,
                            nullptr);

    VkBuffer vertices[]    = {indexBuffer.vertices().handle()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(ctx.commandBuffer, 0, 1, vertices, offsets);
    vkCmdBindIndexBuffer(
        ctx.commandBuffer, indexBuffer.indices().handle(), 0, prim::IndexBuffer::IndexType);

    vkCmdDrawIndexed(ctx.commandBuffer, indexBuffer.indices().size(), 1, 0, 0, 0);
}

std::array<VkDescriptorSetLayoutBinding, 1> Image::DescriptorLayoutBindings() {
    std::array<VkDescriptorSetLayoutBinding, 1> layout{};
    layout[0].binding            = 0;
    layout[0].descriptorCount    = 1;
    layout[0].descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layout[0].pImmutableSamplers = nullptr;
    layout[0].stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
    return layout;
}

} // namespace overlay
} // namespace render
} // namespace bl
