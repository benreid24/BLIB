#include <BLIB/Render/Vulkan/Pipeline.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
Pipeline::Pipeline(Renderer& renderer, PipelineParameters&& params)
: renderer(renderer)
, descriptorGetters(std::move(params.descriptorSetRetrievers))
, descriptorSets(params.descriptorSets.size(), nullptr) {
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

    // Get render pass from cache
    RenderPass& renderPass = renderer.renderPassCache().getRenderPass(params.renderPassId);

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

    // create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = params.descriptorSets.size(); // Optional
    pipelineLayoutInfo.pSetLayouts            = params.descriptorSets.data(); // Optional
    pipelineLayoutInfo.pushConstantRangeCount = params.pushConstants.size();  // Optional
    pipelineLayoutInfo.pPushConstantRanges    = params.pushConstants.data();  // Optional
    if (vkCreatePipelineLayout(
            renderer.vulkanState().device, &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount          = shaderStages.size();
    pipelineInfo.pStages             = shaderStages.data();
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &params.rasterizer;
    pipelineInfo.pMultisampleState   = &params.msaa;
    pipelineInfo.pDepthStencilState  = nullptr; // Optional
    pipelineInfo.pColorBlendState    = &params.colorBlending;
    pipelineInfo.pDynamicState       = &dynamicState;
    pipelineInfo.layout              = layout;
    pipelineInfo.renderPass          = renderPass.rawPass();
    pipelineInfo.subpass             = 0;              // TODO - move to parameters
    pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex   = -1;             // Optional

    if (vkCreateGraphicsPipelines(
            renderer.vulkanState().device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline");
    }

    // Cleanup shader modules
    for (VkPipelineShaderStageCreateInfo shader : shaderStages) {
        vkDestroyShaderModule(renderer.vulkanState().device, shader.module, nullptr);
    }
}

Pipeline::~Pipeline() {
    vkDestroyPipeline(renderer.vulkanState().device, pipeline, nullptr);
    vkDestroyPipelineLayout(renderer.vulkanState().device, layout, nullptr);
}

void Pipeline::bindPipelineAndDescriptors(VkCommandBuffer commandBuffer) {
    for (unsigned int i = 0; i < descriptorGetters.size(); ++i) {
        descriptorSets[i] = descriptorGetters[i]();
    }

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdBindDescriptorSets(commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            layout,
                            0,
                            descriptorSets.size(),
                            descriptorSets.data(),
                            0,
                            nullptr);
}

} // namespace render
} // namespace bl
