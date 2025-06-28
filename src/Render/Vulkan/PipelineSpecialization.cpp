#include <BLIB/Render/Vulkan/PipelineSpecialization.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
PipelineSpecialization::PipelineSpecialization()
: depthStencilSpecialized(false)
, depthStencil{} {}

PipelineSpecialization& PipelineSpecialization::createShaderSpecializations(
    VkShaderStageFlagBits stage, std::uint32_t dataSize, std::uint32_t specializationCount) {
    for (const auto& spec : shaderSpecializations) {
        if (spec.stage == stage) {
            BL_LOG_ERROR << "Shader specialization for stage already exists";
            return *this;
        }
    }

    auto& spec = shaderSpecializations.emplace_back();
    spec.stage = stage;
    spec.storage.resize(dataSize);
    spec.entries.resize(specializationCount, VkSpecializationMapEntry{});
    return *this;
}

PipelineSpecialization& PipelineSpecialization::withShaderOverride(const std::string& path,
                                                                   VkShaderStageFlagBits stage,
                                                                   const std::string& entrypoint) {
    shaderOverrides.emplace_back(path, stage, entrypoint);
    return *this;
}

PipelineSpecialization& PipelineSpecialization::removeShaderSpecialization(
    VkShaderStageFlagBits stage) {
    for (auto it = shaderSpecializations.begin(); it != shaderSpecializations.end(); ++it) {
        if (it->stage == stage) {
            shaderSpecializations.erase(it);
            return *this;
        }
    }
    return *this;
}

PipelineSpecialization& PipelineSpecialization::clearShaderSpecializations() {
    shaderSpecializations.clear();
    return *this;
}

PipelineSpecialization& PipelineSpecialization::withDepthStencil(
    const VkPipelineDepthStencilStateCreateInfo& ds) {
    depthStencilSpecialized = true;
    depthStencil            = ds;
    return *this;
}

PipelineSpecialization& PipelineSpecialization::withSimpleDepthStencil(bool depthTest,
                                                                       bool depthWrite,
                                                                       bool stencilTest,
                                                                       bool stencilWrite) {
    depthStencilSpecialized = true;

    depthStencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable       = depthTest ? VK_TRUE : VK_FALSE;
    depthStencil.depthWriteEnable      = depthWrite ? VK_TRUE : VK_FALSE;
    depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds        = 0.0f;
    depthStencil.maxDepthBounds        = 1.0f;
    depthStencil.stencilTestEnable     = stencilTest || stencilWrite ? VK_TRUE : VK_FALSE;
    depthStencil.front                 = {};
    depthStencil.front.compareOp   = stencilTest ? VK_COMPARE_OP_NOT_EQUAL : VK_COMPARE_OP_ALWAYS;
    depthStencil.front.compareMask = 0xFF;
    depthStencil.front.writeMask   = 0xFF;
    depthStencil.front.depthFailOp = VK_STENCIL_OP_KEEP;
    depthStencil.front.passOp      = stencilWrite ? VK_STENCIL_OP_REPLACE : VK_STENCIL_OP_KEEP;
    depthStencil.front.failOp      = VK_STENCIL_OP_KEEP;
    depthStencil.front.reference   = 1;
    depthStencil.back              = depthStencil.front;

    return *this;
}

PipelineSpecialization& PipelineSpecialization::clearDepthStencil() {
    depthStencilSpecialized = false;
    return *this;
}

} // namespace vk
} // namespace rc
} // namespace bl
