#include <BLIB/Render/Vulkan/PipelineParameters.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Primitives/Vertex.hpp>
#include <stdexcept>

namespace bl
{
namespace rc
{
namespace vk
{
PipelineParameters::PipelineParameters()
: PipelineParameters({Config::RenderPassIds::StandardAttachmentDefault,
                      Config::RenderPassIds::SwapchainDefault}) {}

PipelineParameters::PipelineParameters(const std::initializer_list<std::uint32_t>& rpids)
: primitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
, rasterizer{}
, msaa{}
, colorBlending{}
, depthStencil(nullptr)
, renderPassCount(rpids.size())
, subpass(0)
, localDepthStencil{} {
    if (renderPassCount > Config::MaxRenderPasses) {
        throw std::runtime_error("Too many render passes");
    }
    std::copy(rpids.begin(), rpids.end(), renderPassIds.begin());

    shaders.reserve(4);

    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = VK_CULL_MODE_NONE;
    rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp          = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor    = 0.0f; // Optional

    msaa.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    msaa.sampleShadingEnable   = VK_FALSE;
    msaa.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    msaa.minSampleShading      = 1.0f;     // Optional
    msaa.pSampleMask           = nullptr;  // Optional
    msaa.alphaToCoverageEnable = VK_FALSE; // Optional
    msaa.alphaToOneEnable      = VK_FALSE; // Optional

    colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable     = VK_FALSE;
    colorBlending.logicOp           = VK_LOGIC_OP_COPY; // Optional
    colorBlending.blendConstants[0] = 0.0f;             // Optional
    colorBlending.blendConstants[1] = 0.0f;             // Optional
    colorBlending.blendConstants[2] = 0.0f;             // Optional
    colorBlending.blendConstants[3] = 0.0f;             // Optional

    withDynamicStates({VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VIEWPORT});
}

PipelineParameters& PipelineParameters::forSubpass(std::uint32_t i) {
    subpass = i;
    return *this;
}

PipelineParameters& PipelineParameters::withShaders(const std::string& vert,
                                                    const std::string& frag) {
    shaders.resize(2);

    shaders.front().path       = vert;
    shaders.front().stage      = VK_SHADER_STAGE_VERTEX_BIT;
    shaders.front().entrypoint = "main";

    shaders.back().path       = frag;
    shaders.back().stage      = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaders.back().entrypoint = "main";

    return *this;
}

PipelineParameters& PipelineParameters::addShader(const std::string& path,
                                                  VkShaderStageFlagBits stage,
                                                  const std::string& entrypoint) {
    shaders.emplace_back(path, stage, entrypoint);
    return *this;
}

PipelineParameters& PipelineParameters::withDynamicStates(
    const std::initializer_list<VkDynamicState>& states) {
    dynamicStates.reserve(dynamicStates.size() + states.size());
    for (const VkDynamicState state : states) { dynamicStates.emplace_back(state); }
    return *this;
}

PipelineParameters& PipelineParameters::withPrimitiveType(VkPrimitiveTopology pt) {
    primitiveType = pt;
    return *this;
}

PipelineParameters& PipelineParameters::withRasterizer(
    const VkPipelineRasterizationStateCreateInfo& r) {
    rasterizer = r;
    return *this;
}

PipelineParameters& PipelineParameters::withMSAA(const VkPipelineMultisampleStateCreateInfo& m) {
    msaa = m;
    return *this;
}

PipelineParameters& PipelineParameters::addPushConstantRange(std::uint32_t offset,
                                                             std::uint32_t len,
                                                             VkShaderStageFlags stages) {
#ifdef BLIB_DEBUG
    if (offset % 4 != 0) {
        throw std::runtime_error("Custom push constant offset must be a multiple of 4");
    }
    if (len % 4 != 0) {
        throw std::runtime_error("Custom push constant size must be a multiple of 4");
    }
    if (offset + len > 128) {
        BL_LOG_WARN
            << "Custom push constant exceeds the guaranteed available size of 128 bytes. Offset = "
            << offset << " | len = " << len;
    }
#endif

    layoutParams.addPushConstantRange(offset, len, stages);

    return *this;
}

PipelineParameters& PipelineParameters::addColorAttachmentBlendState(
    const VkPipelineColorBlendAttachmentState& ca) {
    colorAttachmentBlendStates.emplace_back(ca);
    return *this;
}

PipelineParameters& PipelineParameters::withColorBlendStateConfig(VkLogicOp operation,
                                                                  float blendConstant0,
                                                                  float blendConstant1,
                                                                  float blendConstant2,
                                                                  float blendConstant3) {
    colorBlending.logicOpEnable     = VK_TRUE;
    colorBlending.logicOp           = operation;
    colorBlending.blendConstants[0] = blendConstant0;
    colorBlending.blendConstants[1] = blendConstant1;
    colorBlending.blendConstants[2] = blendConstant2;
    colorBlending.blendConstants[3] = blendConstant3;
    return *this;
}

PipelineParameters& PipelineParameters::withDepthStencilState(
    VkPipelineDepthStencilStateCreateInfo* ds) {
    depthStencil = ds;
    return *this;
}

PipelineParameters& PipelineParameters::withSimpleDepthStencil(bool depthTestingEnabled) {
    const auto enable = depthTestingEnabled ? VK_TRUE : VK_FALSE;
    depthStencil      = &localDepthStencil;

    localDepthStencil.sType            = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    localDepthStencil.depthTestEnable  = enable;
    localDepthStencil.depthWriteEnable = enable;
    localDepthStencil.depthCompareOp   = VK_COMPARE_OP_LESS_OR_EQUAL;
    localDepthStencil.depthBoundsTestEnable = VK_FALSE;
    localDepthStencil.minDepthBounds        = 0.0f; // Optional
    localDepthStencil.maxDepthBounds        = 1.0f; // Optional
    localDepthStencil.stencilTestEnable     = VK_FALSE;
    localDepthStencil.front                 = {}; // Optional (Stencil)
    localDepthStencil.back                  = {}; // Optional (Stencil)

    return *this;
}

PipelineParameters&& PipelineParameters::build() {
    if (shaders.empty()) { throw std::runtime_error("Pipeline creation must have shaders"); }
    if (vertexAttributes.empty()) {
        withVertexFormat<3>(prim::Vertex::bindingDescription(),
                            prim::Vertex::attributeDescriptions());
    }
    if (colorAttachmentBlendStates.empty()) {
        colorAttachmentBlendStates.emplace_back();
        auto& ca          = colorAttachmentBlendStates.back();
        ca.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        ca.blendEnable         = VK_TRUE;
        ca.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        ca.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        ca.colorBlendOp        = VK_BLEND_OP_ADD;
        ca.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        ca.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        ca.alphaBlendOp        = VK_BLEND_OP_MAX;
    }
    colorBlending.pAttachments    = colorAttachmentBlendStates.data();
    colorBlending.attachmentCount = colorAttachmentBlendStates.size();

    return std::move(*this);
}

} // namespace vk
} // namespace rc
} // namespace bl
