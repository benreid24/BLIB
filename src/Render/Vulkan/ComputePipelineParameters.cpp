#include <BLIB/Render/Vulkan/ComputePipelineParameters.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
ComputePipelineParameters& ComputePipelineParameters::withShader(const std::string& path,
                                                                 const std::string& entrypoint) {
    shader = ShaderParameters(path, VK_SHADER_STAGE_COMPUTE_BIT, entrypoint);
    return *this;
}

ComputePipelineParameters& ComputePipelineParameters::withShaderConstants(std::uint32_t dataSize,
                                                                          std::uint32_t count) {
    shaderConstantStorage.resize(dataSize);
    shaderConstants.resize(count, VkSpecializationMapEntry{});
    return *this;
}

ComputePipelineParameters& ComputePipelineParameters::addPushConstantRange(std::uint32_t offset,
                                                                           std::uint32_t len) {
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

    layoutParams.addPushConstantRange(offset, len, VK_SHADER_STAGE_COMPUTE_BIT);

    return *this;
}

ComputePipelineParameters&& ComputePipelineParameters::build() { return std::move(*this); }

} // namespace vk
} // namespace rc
} // namespace bl
