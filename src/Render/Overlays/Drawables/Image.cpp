#include <BLIB/Render/Overlays/Drawables/Image.hpp>

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
namespace overlay
{
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

        // create descriptor pool
        VkDescriptorPoolSize samplerPoolSize;
        samplerPoolSize.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerPoolSize.descriptorCount = 1;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes    = &samplerPoolSize;
        poolInfo.maxSets       = 1;
        if (vkCreateDescriptorPool(vs.device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool");
        }

        // allocate descriptor set
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts        = &vs.descriptorSetLayouts.imageOverlay;
        if (vkAllocateDescriptorSets(vs.device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets");
        }

        // create index buffer
        indexBuffer.create(vs, 4, 6);
        indexBuffer.indices().assign({0, 1, 3, 1, 2, 3}, 0);
        indexBuffer.vertices()[0] = Vertex({-1.f, -1.f, 0.f}, {0.f, 0.f});
        indexBuffer.vertices()[1] = Vertex({1.f, -1.f, 0.f}, {1.f, 0.f});
        indexBuffer.vertices()[2] = Vertex({1.f, 1.f, 0.f}, {1.f, 1.f});
        indexBuffer.vertices()[3] = Vertex({-1.f, 1.f, 0.f}, {0.f, 1.f});
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
        const VkDevice device = renderer->vulkanState().device;
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        vkDestroySampler(device, sampler, nullptr);
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
        ctx.commandBuffer, indexBuffer.indices().handle(), 0, IndexBuffer::IndexType);

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
