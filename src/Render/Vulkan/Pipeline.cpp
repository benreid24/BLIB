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
    layout = renderer.pipelineLayoutCache().getLayout(std::move(params.layoutParams));
    for (auto& set : pipelines) { set.fill(nullptr); }
    bl::event::Dispatcher::subscribe(this);
}

Pipeline::~Pipeline() {
    for (auto& set : pipelines) {
        for (const VkPipeline pipeline : set) {
            if (pipeline != nullptr) {
                vkDestroyPipeline(renderer.vulkanState().device, pipeline, nullptr);
            }
        }
    }
}

void Pipeline::bind(VkCommandBuffer commandBuffer, std::uint32_t renderPassId, std::uint32_t spec) {
    if (!pipelines[spec][renderPassId]) { createForRenderPass(renderPassId, spec); }
    vkCmdBindPipeline(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[spec][renderPassId]);
    if (createParams.rasterizer.depthBiasEnable == VK_TRUE) {
        const auto& s = renderer.getSettings();
        vkCmdSetDepthBias(commandBuffer,
                          s.getShadowMapDepthBiasConstantFactor(),
                          s.getShadowMapDepthBiasClamp(),
                          s.getShadowMapDepthBiasSlopeFactor());
    }
}

void Pipeline::createForRenderPass(std::uint32_t rpid, std::uint32_t spec) {
    PipelineSpecialization& specialization =
        spec > 0 && spec <= createParams.specializations.size() ?
            createParams.specializations[spec - 1] :
            createParams.mainSpecialization;
    if (spec > createParams.specializations.size()) {
        BL_LOG_ERROR << "Pipeline being used with invalid specialization: " << spec;
    }

    RenderPass& renderPass = renderer.renderPassCache().getRenderPass(rpid);

    // configure color blending
    BlendParameters& colorBlending = specialization.attachmentBlendingSpecialized ?
                                         specialization.attachmentBlending :
                                         createParams.colorBlending;
    colorBlending.build();

    // sample shading
    const VkBool32 sampleShadingSetting = createParams.msaa.sampleShadingEnable;
    const bool useSampleShading =
        createParams.msaa.sampleShadingEnable == VK_TRUE &&
        renderer.getSettings().getMSAASampleCount() > VK_SAMPLE_COUNT_1_BIT &&
        renderer.vulkanState().physicalDeviceFeatures.sampleRateShading == VK_TRUE;

    // configure msaa
    if (renderPass.getCreateParams().getMSAABehavior() ==
        RenderPassParameters::MSAABehavior::UseSettings) {
        createParams.msaa.rasterizationSamples = renderer.getSettings().getMSAASampleCount();
    }
    else { createParams.msaa.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; }

    const auto findShaderSrc = [this, &specialization, useSampleShading](
                                   const ShaderParameters& src) -> const ShaderParameters& {
        if (useSampleShading) {
            for (const auto& sampleShader : createParams.sampleShaders) {
                if (sampleShader.stage == src.stage) { return sampleShader; }
            }
        }
        for (const auto& shader : specialization.shaderOverrides) {
            if (shader.stage == src.stage) { return shader; }
        }
        return src;
    };

    // Load shaders
    ctr::StaticVector<VkPipelineShaderStageCreateInfo, 5> shaderStages;
    ctr::StaticVector<VkSpecializationInfo, 5> shaderSpecs;
    for (const auto& shaderSrc : createParams.shaders) {
        const auto& shader = findShaderSrc(shaderSrc);
        auto& info         = shaderStages.emplace_back(VkPipelineShaderStageCreateInfo{});
        info.sType         = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info.stage         = static_cast<VkShaderStageFlagBits>(shader.stage);
        info.module        = renderer.vulkanState().shaderCache.loadShader(shader.path);
        info.pName         = shader.entrypoint.c_str();

        for (auto& spec : specialization.shaderSpecializations) {
            if ((spec.stage & shader.stage) != 0) {
                auto& specInfo           = shaderSpecs.emplace_back();
                info.pSpecializationInfo = &specInfo;
                specInfo.dataSize        = static_cast<std::uint32_t>(spec.storage.size());
                specInfo.pData           = spec.storage.data();
                specInfo.mapEntryCount   = static_cast<std::uint32_t>(spec.entries.size());
                specInfo.pMapEntries     = spec.entries.data();
                break;
            }
        }
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
    pipelineInfo.pDepthStencilState  = specialization.depthStencilSpecialized ?
                                           &specialization.depthStencil :
                                           createParams.depthStencil;
    pipelineInfo.pColorBlendState    = &colorBlending.colorBlending;
    pipelineInfo.pDynamicState       = &dynamicState;
    pipelineInfo.layout              = layout->rawLayout();
    pipelineInfo.subpass             = 0;              // TODO - consider dynamic subpass if needed
    pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex   = -1;             // Optional
    pipelineInfo.renderPass          = renderPass.rawPass();

    if (vkCreateGraphicsPipelines(renderer.vulkanState().device,
                                  VK_NULL_HANDLE,
                                  1,
                                  &pipelineInfo,
                                  nullptr,
                                  &pipelines[spec][rpid]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline");
    }
}

void Pipeline::observe(const event::SettingsChanged& event) {
    if (event.setting == event::SettingsChanged::Setting::AntiAliasing) {
        for (std::uint32_t rpid = 0; rpid < pipelines.size(); ++rpid) {
            RenderPass& renderPass = renderer.renderPassCache().getRenderPass(rpid);
            if (renderPass.getCreateParams().getMSAABehavior() ==
                RenderPassParameters::MSAABehavior::UseSettings) {
                for (std::uint32_t spec = 0; spec < pipelines[rpid].size(); ++spec) {
                    if (pipelines[spec][rpid]) {
                        renderer.vulkanState().cleanupManager.add(
                            [device   = renderer.vulkanState().device,
                             pipeline = pipelines[spec][rpid]]() {
                                vkDestroyPipeline(device, pipeline, nullptr);
                            });
                        createForRenderPass(rpid, spec);
                    }
                }
            }
        }
    }
}

} // namespace vk
} // namespace rc
} // namespace bl
