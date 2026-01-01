#include <BLIB/Render/Vulkan/ComputePipeline.hpp>

#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Vulkan/PipelineLayout.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
ComputePipeline::ComputePipeline(Renderer& renderer, std::uint32_t id,
                                 ComputePipelineParameters&& params)
: id(id)
, renderer(renderer)
, createParams(std::move(params)) {
    layout = renderer.pipelineLayoutCache().getLayout(std::move(createParams.layoutParams));

    VkSpecializationInfo specInfo{};
    specInfo.dataSize      = static_cast<std::uint32_t>(createParams.shaderConstantStorage.size());
    specInfo.pData         = createParams.shaderConstantStorage.data();
    specInfo.mapEntryCount = static_cast<std::uint32_t>(createParams.shaderConstants.size());
    specInfo.pMapEntries   = createParams.shaderConstants.data();

    VkComputePipelineCreateInfo createInfo{};
    createInfo.sType       = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    createInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    createInfo.stage.module =
        renderer.vulkanState().getShaderCache().loadShader(createParams.shader.path);
    createInfo.stage.pName               = createParams.shader.entrypoint.c_str();
    createInfo.stage.pSpecializationInfo = &specInfo;
    createInfo.layout                    = layout->rawLayout();

    vkCheck(vkCreateComputePipelines(
        renderer.vulkanState().getDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline));
}

ComputePipeline::~ComputePipeline() {
    vkDestroyPipeline(renderer.vulkanState().getDevice(), pipeline, nullptr);
}

void ComputePipeline::bind(VkCommandBuffer commandBuffer) {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
}

void ComputePipeline::exec(VkCommandBuffer commandBuffer, std::uint32_t groupCountX,
                           std::uint32_t groupCountY, std::uint32_t groupCountZ) {
    vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
}

} // namespace vk
} // namespace rc
} // namespace bl
