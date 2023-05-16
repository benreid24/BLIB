#include <BLIB/Render/Scenes/PostFX.hpp>

#include <BLIB/Render/Descriptors/Builtin/PostFXDescriptorSetFactory.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
namespace scene
{
namespace
{
const prim::Vertex vertices[4] = {prim::Vertex({-1.f, -1.f, 0.f}, {0.f, 0.f}),
                                  prim::Vertex({1.f, -1.f, 0.f}, {1.f, 0.f}),
                                  prim::Vertex({1.f, 1.f, 0.f}, {1.f, 1.f}),
                                  prim::Vertex({-1.f, 1.f, 0.f}, {0.f, 1.f})};

const std::uint32_t indices[6] = {0, 1, 3, 1, 2, 3};

VkDescriptorSetLayoutCreateInfo makeCreateInfo() {
    static const auto layoutBindings = PostFX::DescriptorLayoutBindings();
    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = static_cast<std::uint32_t>(layoutBindings.size());
    createInfo.pBindings    = layoutBindings.data();
    return createInfo;
}
} // namespace

std::array<VkDescriptorSetLayoutBinding, 1> PostFX::DescriptorLayoutBindings() {
    std::array<VkDescriptorSetLayoutBinding, 1> layout{};
    layout[0].binding            = 0;
    layout[0].descriptorCount    = 1;
    layout[0].descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layout[0].pImmutableSamplers = nullptr;
    layout[0].stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
    return layout;
}

PostFX::PostFX(Renderer& renderer)
: renderer(renderer) {
    vk::VulkanState& vs = renderer.vulkanState();

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

    // allocate descriptor sets
    const VkDescriptorSetLayout setLayout = renderer.descriptorFactoryCache()
                                                .getFactory<ds::PostFXDescriptorSetFactory>()
                                                ->getDescriptorLayout();
    const VkDescriptorSetLayoutCreateInfo createInfo = makeCreateInfo();
    descriptorSets.init(renderer.vulkanState(), [](VkDescriptorSet&) {});
    std::array<VkDescriptorSetLayout, Config::MaxConcurrentFrames> descriptorLayouts;
    descriptorLayouts.fill(setLayout);
    std::array<const VkDescriptorSetLayoutCreateInfo*, Config::MaxConcurrentFrames>
        descriptorCreateInfos;
    descriptorCreateInfos.fill(&createInfo);
    descriptorSetAllocHandle = vs.descriptorPool.allocate(descriptorCreateInfos.data(),
                                                          descriptorLayouts.data(),
                                                          descriptorSets.rawData(),
                                                          Config::MaxConcurrentFrames);

    // create index buffer
    indexBuffer.create(vs, 4, 6);
    indexBuffer.indices().configureWrite(indices, 0, 6 * sizeof(std::uint32_t));
    indexBuffer.vertices().configureWrite(vertices, 0, 4 * sizeof(prim::Vertex));
    indexBuffer.sendToGPU();

    // fetch initial pipeline
    usePipeline(Config::PipelineIds::PostFXBase);
}

void PostFX::bindImages(vk::PerFrame<vk::StandardImageBuffer>& images) {
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
    vk::VulkanState& vs = renderer.vulkanState();

    const VkDescriptorSetLayoutCreateInfo createInfo = makeCreateInfo();
    std::array<const VkDescriptorSetLayoutCreateInfo*, Config::MaxConcurrentFrames>
        descriptorCreateInfos;
    descriptorCreateInfos.fill(&createInfo);
    vs.descriptorPool.release(descriptorSetAllocHandle,
                              descriptorCreateInfos.data(),
                              descriptorSets.rawData(),
                              Config::MaxConcurrentFrames);
    vkDestroySampler(vs.device, sampler, nullptr);
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

    VkBuffer vertexBuffer[] = {indexBuffer.vertices().handle()};
    VkDeviceSize offsets[]  = {0};
    vkCmdBindVertexBuffers(cb, 0, 1, vertexBuffer, offsets);
    vkCmdBindIndexBuffer(cb, indexBuffer.indices().handle(), 0, prim::IndexBuffer::IndexType);

    onRender(cb);
    vkCmdDrawIndexed(cb, indexBuffer.indexCount(), 1, 0, 0, 0);
}

void PostFX::onRender(VkCommandBuffer) {
    // do nothing
}

} // namespace scene
} // namespace render
} // namespace bl
