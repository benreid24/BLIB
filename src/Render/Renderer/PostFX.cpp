#include <BLIB/Render/Renderer/PostFX.hpp>

#include <BLIB/Render/Overlays/Drawables/Image.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
std::array<VkDescriptorSetLayoutBinding, 1> PostFX::DescriptorLayoutBindings() {
    return overlay::Image::DescriptorLayoutBindings();
}

PostFX::PostFX(Renderer& renderer)
: renderer(renderer) {
    VulkanState& vs = renderer.vulkanState();

    // create sampler
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter               = VK_FILTER_NEAREST;
    samplerInfo.minFilter               = VK_FILTER_NEAREST;
    samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerInfo.anisotropyEnable        = VK_TRUE;
    samplerInfo.maxAnisotropy           = vs.physicalDeviceProperties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor             = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
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
    poolInfo.maxSets       = Config::MaxConcurrentFrames;
    if (vkCreateDescriptorPool(vs.device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool");
    }

    // allocate descriptor sets
    descriptorSets.init(renderer.vulkanState(), [](VkDescriptorSet&) {});
    std::array<VkDescriptorSetLayout, Config::MaxConcurrentFrames> descriptorLayouts;
    descriptorLayouts.fill(vs.descriptorSetLayouts.imageOverlay);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = descriptorPool;
    allocInfo.descriptorSetCount = descriptorLayouts.size();
    allocInfo.pSetLayouts        = descriptorLayouts.data();
    if (vkAllocateDescriptorSets(vs.device, &allocInfo, descriptorSets.rawData()) != VK_SUCCESS) {
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

void PostFX::bindImages(PerFrame<StandardImageBuffer>& images) {
    std::array<VkDescriptorImageInfo, Config::MaxConcurrentFrames> imageInfos{};
    for (unsigned int i = 0; i < images.size(); ++i) {
        imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfos[i].imageView   = images.getRaw(i).attachmentSet().colorImageView();
        imageInfos[i].sampler     = sampler;
    }

    std::array<VkWriteDescriptorSet, Config::MaxConcurrentFrames> descriptorWrites{};
    for (unsigned int i = 0; i < images.size(); ++i) {
        descriptorWrites[i].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[i].dstSet          = descriptorSets.getRaw(i);
        descriptorWrites[i].dstBinding      = 0;
        descriptorWrites[i].dstArrayElement = 0;
        descriptorWrites[i].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[i].descriptorCount = 1;
        descriptorWrites[i].pImageInfo      = &imageInfos[i];
    }

    vkUpdateDescriptorSets(renderer.vulkanState().device,
                           descriptorWrites.size(),
                           descriptorWrites.data(),
                           0,
                           nullptr);
}

PostFX::~PostFX() {
    const VkDevice device = renderer.vulkanState().device;

    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    vkDestroySampler(device, sampler, nullptr);
    indexBuffer.destroy();
}

void PostFX::usePipeline(std::uint32_t pid) {
    pipeline = &renderer.pipelineCache().getPipeline(pid);
}

void PostFX::update(float) {
    // do nothing
}

void PostFX::compositeScene(VkCommandBuffer cb) {
    vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->rawPipeline());
    vkCmdBindDescriptorSets(cb,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline->pipelineLayout(),
                            0,
                            1,
                            &descriptorSets.current(),
                            0,
                            nullptr);

    VkBuffer vertices[]    = {indexBuffer.vertices().handle()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cb, 0, 1, vertices, offsets);
    vkCmdBindIndexBuffer(cb, indexBuffer.indices().handle(), 0, IndexBuffer::IndexType);

    onRender(cb);
    vkCmdDrawIndexed(cb, indexBuffer.indices().size(), 1, 0, 0, 0);
}

void PostFX::onRender(VkCommandBuffer) {
    // do nothing
}

} // namespace render
} // namespace bl
