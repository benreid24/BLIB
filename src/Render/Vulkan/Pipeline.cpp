#include <BLIB/Render/Vulkan/Pipeline.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace gfx
{
namespace vk
{
Pipeline::Pipeline(Renderer& renderer, PipelineParameters&& params)
: renderer(renderer) {
    pipelines.fill(nullptr);

    // create or fetch layout
    layout = renderer.pipelineLayoutCache().getLayout(std::move(params.layoutParams));

    // Load shaders
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    shaderStages.reserve(params.shaders.size());
    for (const auto& shader : params.shaders) {
        shaderStages.emplace_back();
        shaderStages.back().sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages.back().stage  = shader.stage;
        shaderStages.back().module = renderer.vulkanState().createShaderModule(shader.path);
        shaderStages.back().pName  = shader.entrypoint.c_str();
    }

    // Configure vertices
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = 1;
    vertexInputInfo.pVertexBindingDescriptions      = &params.vertexBinding;
    vertexInputInfo.vertexAttributeDescriptionCount = params.vertexAttributes.size();
    vertexInputInfo.pVertexAttributeDescriptions    = params.vertexAttributes.data();

    // Configure primitives
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = params.primitiveType;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Always have 1 dynamic viewport and scissor
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount  = 1;

    // Dynamic states
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = params.dynamicStates.size();
    dynamicState.pDynamicStates    = params.dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount          = shaderStages.size();
    pipelineInfo.pStages             = shaderStages.data();
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &params.rasterizer;
    pipelineInfo.pMultisampleState   = &params.msaa;
    pipelineInfo.pDepthStencilState  = params.depthStencil;
    pipelineInfo.pColorBlendState    = &params.colorBlending;
    pipelineInfo.pDynamicState       = &dynamicState;
    pipelineInfo.layout              = layout->rawLayout();
    pipelineInfo.subpass             = params.subpass;
    pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex   = -1;             // Optional

    for (std::uint32_t i = 0; i < params.renderPassCount; ++i) {
        const auto rpid = params.renderPassIds[i];
        if (rpid >= Config::MaxRenderPasses) { throw std::runtime_error("Invalid RenderPAss id"); }
        RenderPass& renderPass  = renderer.renderPassCache().getRenderPass(rpid);
        pipelineInfo.renderPass = renderPass.rawPass();

        if (vkCreateGraphicsPipelines(renderer.vulkanState().device,
                                      VK_NULL_HANDLE,
                                      1,
                                      &pipelineInfo,
                                      nullptr,
                                      &pipelines[rpid]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline");
        }
    }

    // Cleanup shader modules
    for (VkPipelineShaderStageCreateInfo shader : shaderStages) {
        vkDestroyShaderModule(renderer.vulkanState().device, shader.module, nullptr);
    }
}

Pipeline::~Pipeline() {
    for (auto pipeline : pipelines) {
        if (pipeline != nullptr) {
            vkDestroyPipeline(renderer.vulkanState().device, pipeline, nullptr);
        }
    }
}

void Pipeline::bind(VkCommandBuffer commandBuffer, std::uint32_t renderPassId) {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[renderPassId]);
}

} // namespace vk
} // namespace gfx
} // namespace bl
