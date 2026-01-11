#ifndef BLIB_RENDER_VULKAN_PIPELINESPECIALIZATION_HPP
#define BLIB_RENDER_VULKAN_PIPELINESPECIALIZATION_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Vulkan/BlendParameters.hpp>
#include <BLIB/Render/Vulkan/ShaderParameters.hpp>
#include <BLIB/Vulkan.hpp>
#include <cstdint>
#include <vector>

namespace bl
{
namespace rc
{
namespace vk
{
class Pipeline;
class PipelineParameters;

/**
 * @brief Parameters for pipeline specializations. Pipeline specializations are basic permutations
 *        of primary pipelines with small differences in settings and or shader constants
 *
 * @ingroup Renderer
 */
class PipelineSpecialization {
public:
    /**
     * @brief Creates the specialization parameters
     */
    PipelineSpecialization();

    /**
     * @brief Creates the info for a shader stage specialization. Must be called before setting
     *        shader constants with setShaderSpecializationValue()
     *
     * @param stage The shader stage to specialize
     * @param dataSize The size of the combined constants
     * @param specializationCount The number of shader constants
     *
     * @return A reference to this object
     */
    PipelineSpecialization& createShaderSpecializations(VkShaderStageFlags stage,
                                                        std::uint32_t dataSize,
                                                        std::uint32_t specializationCount);

    /**
     * @brief Sets the value of a shader constant
     *
     * @tparam T The type of value to set
     * @param stage The shader stage to specialize
     * @param entry The index of the specialization map entry to write
     * @param offset The byte offset to write the constant at
     * @param value The value to set
     * @return A reference to this object
     */
    template<typename T>
    PipelineSpecialization& setShaderSpecializationValue(VkShaderStageFlags stage,
                                                         std::uint32_t entry, std::uint32_t offset,
                                                         T value);

    /**
     * @brief Overrides a shader in the underlying pipeline. The pipeline layout must be compatible
     *
     * @param path The resource path of the shader to override
     * @param stage The stage of the shader to override
     * @param entrypoint The entrypoint inside the shader to run
     * @return A reference to this object
     */
    PipelineSpecialization& withShaderOverride(const std::string& path, VkShaderStageFlags stage,
                                               const std::string& entrypoint = "main");

    /**
     * @brief Removes the shader specialization for the given stage
     *
     * @param stage The stage to remove the specialization for
     * @return A reference to this object
     */
    PipelineSpecialization& removeShaderSpecialization(VkShaderStageFlags stage);

    /**
     * @brief Removes all shader specializations
     *
     * @return A reference to this object
     */
    PipelineSpecialization& clearShaderSpecializations();

    /**
     * @brief Sets the depth stencil state to specialize with
     *
     * @param depthStencil The depth stencil override
     * @return A reference to this object
     */
    PipelineSpecialization& withDepthStencil(
        const VkPipelineDepthStencilStateCreateInfo& depthStencil);

    /**
     * @brief Sets the depth stencil override state
     *
     * @param depthTest Whether depth testing is enabled
     * @param depthWrite Whether depth writing is enabled
     * @param stencilTest Whether stencil testing is enabled
     * @param stencilWrite Whether stencil writing is enabled
     * @return A reference to this object
     */
    PipelineSpecialization& withSimpleDepthStencil(bool depthTest, bool depthWrite = true,
                                                   bool stencilTest  = false,
                                                   bool stencilWrite = false);

    /**
     * @brief Removes the depth stencil override
     *
     * @return A reference to this object
     */
    PipelineSpecialization& clearDepthStencil();

    /**
     * @brief Overrides the attachment blending
     *
     * @param blendConfig The attachment blend states
     * @return A reference to this object
     */
    PipelineSpecialization& withBlendConfig(const BlendParameters& blendConfig);

    /**
     * @brief Removes any override for attachment blending
     */
    PipelineSpecialization& clearBlendConfig();

private:
    struct ShaderSpecialization {
        VkShaderStageFlags stage;
        std::vector<char> storage;
        std::vector<VkSpecializationMapEntry> entries;
    };

    std::vector<ShaderParameters> shaderOverrides;
    std::vector<ShaderSpecialization> shaderSpecializations;
    bool depthStencilSpecialized;
    VkPipelineDepthStencilStateCreateInfo depthStencil;
    bool attachmentBlendingSpecialized;
    BlendParameters attachmentBlending;

    friend class Pipeline;
    friend class PipelineParameters;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
PipelineSpecialization& PipelineSpecialization::setShaderSpecializationValue(
    VkShaderStageFlags stage, std::uint32_t entry, std::uint32_t offset, T value) {
    for (auto& spec : shaderSpecializations) {
        if (spec.stage == stage) {
            if (offset + sizeof(T) > spec.storage.size()) {
                BL_LOG_ERROR << "Shader specialization value exceeds buffer. Ignoring";
                return *this;
            }
            if (entry >= spec.entries.size()) {
                BL_LOG_ERROR << "Shader specialization entry exceeds declared map size. Ignoring";
                return *this;
            }
            *static_cast<T*>(static_cast<void*>(spec.storage.data() + offset)) = value;
            spec.entries[entry].offset                                         = offset;
            spec.entries[entry].size                                           = sizeof(T);
            spec.entries[entry].constantID                                     = entry;
            return *this;
        }
    }

    BL_LOG_ERROR << "Specialization created for undeclared shader stage";

    return *this;
}

} // namespace vk
} // namespace rc
} // namespace bl

#endif
