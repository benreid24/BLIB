#include <BLIB/Render/Vulkan/PipelineParameters.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Primitives/Vertex.hpp>
#include <BLIB/Util/HashCombine.hpp>
#include <stdexcept>

namespace bl
{
namespace rc
{
namespace vk
{
PipelineParameters::PipelineParameters()
: primitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
, rasterizer{}
, colorBlending{}
, depthStencil(nullptr)
, msaa{}
, localDepthClipping{}
, localDepthStencil{} {
    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = VK_CULL_MODE_NONE;
    rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp          = 0.0f;
    rasterizer.depthBiasSlopeFactor    = 0.0f;

    msaa.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    msaa.sampleShadingEnable   = VK_FALSE;
    msaa.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    msaa.minSampleShading      = 0.0f;
    msaa.pSampleMask           = nullptr;
    msaa.alphaToCoverageEnable = VK_FALSE;
    msaa.alphaToOneEnable      = VK_FALSE;

    withDynamicStates({VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VIEWPORT});
}

PipelineParameters::PipelineParameters(const PipelineParameters& copy)
: layoutParams(copy.layoutParams)
, shaders(copy.shaders)
, dynamicStates(copy.dynamicStates)
, vertexBinding(copy.vertexBinding)
, vertexAttributes(copy.vertexAttributes)
, primitiveType(copy.primitiveType)
, rasterizer(copy.rasterizer)
, msaa(copy.msaa)
, colorBlending(copy.colorBlending)
, depthStencil(&localDepthStencil)
, specializations(copy.specializations)
, localDepthClipping(copy.localDepthClipping)
, localDepthStencil(copy.depthStencil ? *copy.depthStencil : copy.localDepthStencil) {
    if (static_cast<const void*>(&copy.localDepthClipping) == copy.rasterizer.pNext) {
        rasterizer.pNext = &localDepthClipping;
    }
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

PipelineParameters& PipelineParameters::withShader(const std::string& path,
                                                   VkShaderStageFlagBits stage,
                                                   const std::string& entrypoint) {
    for (auto& shader : shaders) {
        if (shader.stage == stage) {
            shader.path       = path;
            shader.entrypoint = entrypoint;
            return *this;
        }
    }
    shaders.emplace_back(path, stage, entrypoint);
    return *this;
}

PipelineParameters& PipelineParameters::withSampleShader(const std::string& path,
                                                         VkShaderStageFlagBits stage,
                                                         const std::string& entrypoint) {
    for (auto& shader : sampleShaders) {
        if (shader.stage == stage) {
            shader.path       = path;
            shader.entrypoint = entrypoint;
            return *this;
        }
    }
    sampleShaders.emplace_back(path, stage, entrypoint);
    return *this;
}

PipelineParameters& PipelineParameters::removeShader(VkShaderStageFlagBits stage) {
    for (unsigned int i = 0; i < shaders.size(); ++i) {
        if (shaders[i].stage == stage) {
            shaders.erase(i);
            return *this;
        }
    }
    return *this;
}

PipelineParameters& PipelineParameters::withDynamicStates(
    const std::initializer_list<VkDynamicState>& states) {
    dynamicStates.clear();
    for (const VkDynamicState state : states) { dynamicStates.emplace_back(state); }
    return *this;
}

PipelineParameters& PipelineParameters::addDynamicStates(
    const std::initializer_list<VkDynamicState>& states) {
    for (const VkDynamicState state : states) { dynamicStates.emplace_back(state); }
    return *this;
}

PipelineParameters& PipelineParameters::withPrimitiveType(VkPrimitiveTopology pt) {
    primitiveType = pt;
    return *this;
}

PipelineParameters& PipelineParameters::withRasterizer(
    const VkPipelineRasterizationStateCreateInfo& r) {
    if (rasterizer.pNext == static_cast<void*>(&localDepthClipping)) {
        BL_LOG_WARN << "Possibly clobbering depth clipping state. withRasterizer() should be "
                       "called prior to withEnableDepthClipping()";
    }
    rasterizer = r;
    return *this;
}

PipelineParameters& PipelineParameters::withEnableDepthClipping() {
    localDepthClipping = {};
    localDepthClipping.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_DEPTH_CLIP_STATE_CREATE_INFO_EXT;
    localDepthClipping.depthClipEnable = VK_TRUE;
    rasterizer.pNext                   = &localDepthClipping;
    return *this;
}

PipelineParameters& PipelineParameters::withSampleShading(bool enabled, float minSampleShading) {
    msaa.sampleShadingEnable = enabled ? VK_TRUE : VK_FALSE;
    msaa.minSampleShading    = minSampleShading;
    return *this;
}

PipelineParameters& PipelineParameters::removeDescriptorSet(unsigned int i) {
    layoutParams.removeDescriptorSet(i);
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

PipelineParameters& PipelineParameters::withBlendConfig(const BlendParameters& blendConfig) {
    colorBlending = blendConfig;
    return *this;
}

PipelineParameters& PipelineParameters::withDepthStencilState(
    VkPipelineDepthStencilStateCreateInfo* ds) {
    depthStencil = ds;
    return *this;
}

PipelineParameters& PipelineParameters::withSimpleDepthStencil(bool depthTest, bool depthWrite,
                                                               bool stencilTest,
                                                               bool stencilWrite) {
    depthStencil = &localDepthStencil;

    localDepthStencil.sType            = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    localDepthStencil.depthTestEnable  = depthTest ? VK_TRUE : VK_FALSE;
    localDepthStencil.depthWriteEnable = depthWrite ? VK_TRUE : VK_FALSE;
    localDepthStencil.depthCompareOp   = VK_COMPARE_OP_LESS_OR_EQUAL;
    localDepthStencil.depthBoundsTestEnable = VK_FALSE;
    localDepthStencil.minDepthBounds        = 0.0f;
    localDepthStencil.maxDepthBounds        = 1.0f;
    localDepthStencil.stencilTestEnable     = stencilTest || stencilWrite ? VK_TRUE : VK_FALSE;
    localDepthStencil.front                 = {};
    localDepthStencil.front.compareOp =
        stencilTest ? VK_COMPARE_OP_NOT_EQUAL : VK_COMPARE_OP_ALWAYS;
    localDepthStencil.front.compareMask = 0xFF;
    localDepthStencil.front.writeMask   = 0xFF;
    localDepthStencil.front.depthFailOp = VK_STENCIL_OP_KEEP;
    localDepthStencil.front.passOp      = stencilWrite ? VK_STENCIL_OP_REPLACE : VK_STENCIL_OP_KEEP;
    localDepthStencil.front.failOp      = VK_STENCIL_OP_KEEP;
    localDepthStencil.front.reference   = 1;
    localDepthStencil.back              = localDepthStencil.front;

    return *this;
}

PipelineParameters& PipelineParameters::withDeclareSpecializations(std::uint32_t c) {
    specializations.resize(c, PipelineSpecialization());
    return *this;
}

PipelineParameters& PipelineParameters::withSpecialization(
    std::uint32_t id, const PipelineSpecialization& specialization) {
    if (id == 0) {
        mainSpecialization = specialization;
        return *this;
    }

    const std::uint32_t i = id - 1;
    if (i >= specializations.size()) { throw std::runtime_error("Specialization id out of range"); }

    specializations[i] = specialization;
    return *this;
}

PipelineParameters&& PipelineParameters::build() {
    if (shaders.empty()) { throw std::runtime_error("Pipeline creation must have shaders"); }
    if (vertexAttributes.empty()) {
        withVertexFormat<3>(prim::Vertex::bindingDescription(),
                            prim::Vertex::attributeDescriptions());
    }

    return std::move(*this);
}

bool PipelineParameters::handleChange(Renderer& renderer,
                                      const event::SettingsChanged& changeEvent) {
    bool changed = false;
    for (auto& modifier : dynamicModifiers) {
        if (modifier(renderer, *this, changeEvent)) { changed = true; }
    }
    return changed;
}

bool PipelineParameters::operator==(const PipelineParameters& right) const {
    if (layoutParams != right.layoutParams) { return false; }
    if (shaders.size() != right.shaders.size()) { return false; }
    for (unsigned int i = 0; i < shaders.size(); ++i) {
        if (shaders[i].stage != right.shaders[i].stage) { return false; }
        if (shaders[i].path != right.shaders[i].path) { return false; }
        if (shaders[i].entrypoint != right.shaders[i].entrypoint) { return false; }
    }

    if (dynamicStates.size() != right.dynamicStates.size()) { return false; }
    for (unsigned int i = 0; i < dynamicStates.size(); ++i) {
        if (dynamicStates[i] != right.dynamicStates[i]) { return false; }
    }

    if (vertexBinding.binding != right.vertexBinding.binding) { return false; }
    if (vertexAttributes.size() != right.vertexAttributes.size()) { return false; }
    for (unsigned int i = 0; i < vertexAttributes.size(); ++i) {
        if (vertexAttributes[i].binding != right.vertexAttributes[i].binding) { return false; }
        if (vertexAttributes[i].location != right.vertexAttributes[i].location) { return false; }
        if (vertexAttributes[i].format != right.vertexAttributes[i].format) { return false; }
        if (vertexAttributes[i].offset != right.vertexAttributes[i].offset) { return false; }
    }

    if (primitiveType != right.primitiveType) { return false; }
    if (rasterizer.flags != right.rasterizer.flags) { return false; }
    if (rasterizer.depthClampEnable != right.rasterizer.depthClampEnable) { return false; }
    if (rasterizer.rasterizerDiscardEnable != right.rasterizer.rasterizerDiscardEnable) {
        return false;
    }
    if (rasterizer.polygonMode != right.rasterizer.polygonMode) { return false; }
    if (rasterizer.lineWidth != right.rasterizer.lineWidth) { return false; }
    if (rasterizer.cullMode != right.rasterizer.cullMode) { return false; }
    if (rasterizer.frontFace != right.rasterizer.frontFace) { return false; }
    if (rasterizer.depthBiasEnable != right.rasterizer.depthBiasEnable) { return false; }
    if (rasterizer.depthBiasConstantFactor != right.rasterizer.depthBiasConstantFactor) {
        return false;
    }
    if (rasterizer.depthBiasClamp != right.rasterizer.depthBiasClamp) { return false; }
    if (rasterizer.depthBiasSlopeFactor != right.rasterizer.depthBiasSlopeFactor) { return false; }

    if (msaa.flags != right.msaa.flags) { return false; }
    if (msaa.rasterizationSamples != right.msaa.rasterizationSamples) { return false; }
    if (msaa.sampleShadingEnable != right.msaa.sampleShadingEnable) { return false; }
    if (msaa.minSampleShading != right.msaa.minSampleShading) { return false; }
    if (msaa.pSampleMask != right.msaa.pSampleMask) { return false; }
    if (msaa.alphaToCoverageEnable != right.msaa.alphaToCoverageEnable) { return false; }
    if (msaa.alphaToOneEnable != right.msaa.alphaToOneEnable) { return false; }

    if (colorBlending != right.colorBlending) { return false; }

    if (depthStencil) {
        if (depthStencil != right.depthStencil) { return false; }
    }
    else {
        if (localDepthStencil.flags != right.localDepthStencil.flags) { return false; }
        if (localDepthStencil.depthTestEnable != right.localDepthStencil.depthTestEnable) {
            return false;
        }
        if (localDepthStencil.depthWriteEnable != right.localDepthStencil.depthWriteEnable) {
            return false;
        }
        if (localDepthStencil.depthCompareOp != right.localDepthStencil.depthCompareOp) {
            return false;
        }
        if (localDepthStencil.depthBoundsTestEnable !=
            right.localDepthStencil.depthBoundsTestEnable) {
            return false;
        }
        if (localDepthStencil.minDepthBounds != right.localDepthStencil.minDepthBounds) {
            return false;
        }
        if (localDepthStencil.maxDepthBounds != right.localDepthStencil.maxDepthBounds) {
            return false;
        }
        if (localDepthStencil.stencilTestEnable != right.localDepthStencil.stencilTestEnable) {
            return false;
        }
    }

    return true;
}

bool PipelineParameters::operator!=(const PipelineParameters& right) const {
    return !this->operator==(right);
}

} // namespace vk
} // namespace rc
} // namespace bl

namespace std
{
std::size_t std::hash<bl::rc::vk::PipelineParameters>::operator()(
    const bl::rc::vk::PipelineParameters& params) const {
    // hashing the shaders is generally enough as we do not need speed nor will we have many
    // pipelines sharing the same set of shaders
    std::size_t result = hash<size_t>()(params.shaders.size());
    for (unsigned int i = 0; i < params.shaders.size(); ++i) {
        const size_t nh = hash<std::string>()(params.shaders[i].path);
        result          = bl::util::hashCombine(result, nh);
    }
    return result;
}
} // namespace std
