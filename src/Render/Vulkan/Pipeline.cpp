#include <BLIB/Render/Vulkan/Pipeline.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
Pipeline::Pipeline(Renderer& renderer, std::uint32_t id, PipelineParameters&& params)
: id(id)
, renderer(renderer)
, createParams(params) {
    pipelines.fill(nullptr);

    // create or fetch layout
    layout = renderer.pipelineLayoutCache().getLayout(std::move(params.layoutParams));

    // fill pipelines with nullptr
    pipelines.fill(nullptr);
}

Pipeline::~Pipeline() {
    for (auto pipeline : pipelines) {
        if (pipeline != nullptr) {
            vkDestroyPipeline(renderer.vulkanState().device, pipeline, nullptr);
        }
    }
}

void Pipeline::bind(VkCommandBuffer commandBuffer, std::uint32_t renderPassId) {
    if (!pipelines[renderPassId]) { createForRenderPass(renderPassId); }
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[renderPassId]);
}

void Pipeline::createForRenderPass(std::uint32_t rpid) {
    // Load shaders
    ctr::StaticVector<VkPipelineShaderStageCreateInfo, 5> shaderStages;
    for (const auto& shader : createParams.shaders) {
        shaderStages.push_back({});
        shaderStages.back().sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages.back().stage  = shader.stage;
        shaderStages.back().module = renderer.vulkanState().shaderCache.loadShader(shader.path);
        shaderStages.back().pName  = shader.entrypoint.c_str();
    }

    // Configure vertices
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = 1;
    vertexInputInfo.pVertexBindingDescriptions      = &createParams.vertexBinding;
    vertexInputInfo.vertexAttributeDescriptionCount = createParams.vertexAttributes.size();
    vertexInputInfo.pVertexAttributeDescriptions    = createParams.vertexAttributes.data();

    // Configure primitives
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = createParams.primitiveType;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Always have 1 dynamic viewport and scissor
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount  = 1;

    // Dynamic states
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = createParams.dynamicStates.size();
    dynamicState.pDynamicStates    = createParams.dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount          = shaderStages.size();
    pipelineInfo.pStages             = shaderStages.data();
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &createParams.rasterizer;
    pipelineInfo.pMultisampleState   = &createParams.msaa;
    pipelineInfo.pDepthStencilState  = createParams.depthStencil;
    pipelineInfo.pColorBlendState    = &createParams.colorBlending;
    pipelineInfo.pDynamicState       = &dynamicState;
    pipelineInfo.layout              = layout->rawLayout();
    pipelineInfo.subpass             = 0;              // TODO - consider dynamic subpass if needed
    pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex   = -1;             // Optional

    pipelineInfo.renderPass = renderer.renderPassCache().getRenderPass(rpid).rawPass();
    if (vkCreateGraphicsPipelines(renderer.vulkanState().device,
                                  VK_NULL_HANDLE,
                                  1,
                                  &pipelineInfo,
                                  nullptr,
                                  &pipelines[rpid]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline");
    }
}

} // namespace vk
} // namespace rc
} // namespace bl
