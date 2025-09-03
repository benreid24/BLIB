#ifndef BLIB_RENDER_VULKAN_COMPUTEPIPELINEPARAMETERS_HPP
#define BLIB_RENDER_VULKAN_COMPUTEPIPELINEPARAMETERS_HPP

#include <BLIB/Containers/StaticVector.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>
#include <BLIB/Render/Vulkan/PipelineLayout.hpp>
#include <BLIB/Render/Vulkan/ShaderParameters.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
class ComputePipeline;

/**
 * @brief Parameters used to create a compute pipeline
 *
 * @ingroup Renderer
 */
class ComputePipelineParameters {
public:
    /**
     * @brief Creates the parameters with default values
     */
    ComputePipelineParameters() = default;

    /**
     * @brief Copes the parameters
     */
    ComputePipelineParameters(const ComputePipelineParameters&) = default;

    /**
     * @brief Copes the parameters
     */
    ComputePipelineParameters(ComputePipelineParameters&&) = default;

    /**
     * @brief Configures the pipeline to use the given shader
     *
     * @param path The path to the shader resource
     * @param entrypoint The entrypoint in the shader to run
     * @return A reference to this object
     */
    ComputePipelineParameters& withShader(const std::string& path,
                                          const std::string& entrypoint = "main");

    /**
     * @brief Sets up compilation constants for the compute shader
     *
     * @param dataSize The size of the combined constants
     * @param specializationCount The number of specialization constants
     * @return A reference to this object
     */
    ComputePipelineParameters& withShaderConstants(std::uint32_t dataSize,
                                                   std::uint32_t specializationCount);

    /**
     * @brief Sets the value of a shader constant
     *
     * @tparam T The type of value to set
     * @param entry The index of the specialization map entry to write
     * @param offset The byte offset to write the constant at
     * @param value The value to set
     * @return A reference to this object
     */
    template<typename T>
    ComputePipelineParameters& withShaderConstant(std::uint32_t entry, std::uint32_t offset,
                                                  T value) {
        if (offset + sizeof(T) > shaderConstantStorage.size()) {
            BL_LOG_ERROR << "Shader specialization value exceeds buffer. Ignoring";
            return *this;
        }
        if (entry >= shaderConstants.size()) {
            BL_LOG_ERROR << "Shader specialization entry exceeds declared map size. Ignoring";
            return *this;
        }
        *static_cast<T*>(static_cast<void*>(shaderConstantStorage.data() + offset)) = value;
        shaderConstants[entry].offset                                               = offset;
        shaderConstants[entry].size                                                 = sizeof(T);
        shaderConstants[entry].constantID                                           = entry;
        return *this;
    }

    /**
     * @brief Sets a shader constant at the specified entry index with the given value. Tries to
     *        guess the offset to write at based on previous entries, so this must be called in
     *        order of entry index
     *
     * @tparam T The type of the value to assign to the shader constant.
     * @param entry The index of the shader constant entry to set.
     * @param value The value to assign to the shader constant.
     * @return A reference to this object
     */
    template<typename T>
    ComputePipelineParameters& withShaderConstant(std::uint32_t entry, T value) {
        const std::uint32_t offset =
            entry > 0 ? shaderConstants[entry - 1].offset + shaderConstants[entry - 1].size : 0;
        return withShaderConstant(entry, offset, value);
    }

    /**
     * @brief Adds a descriptor set factory to the pipeline. The factory is allocated here and then
     *        deduped on actual pipeline creation. Descriptor set indices are determined by the
     *        order they are added in
     *
     * @tparam TFactory The type of descriptor set factory to create and add
     * @tparam ...TArgs Types of arguments to the factory's constructor
     * @param ...args Arguments to the factory's constructor
     * @return A reference to this object
     */
    template<typename TFactory, typename... TArgs>
    ComputePipelineParameters& addDescriptorSet(TArgs&&... args) {
        layoutParams.addDescriptorSet<TFactory>(std::forward<TArgs>(args)...);
        return *this;
    }

    /**
     * @brief Like addDescriptorSet but replaces an existing descriptor set factory at the given
     *        index (which must be valid)
     *
     * @tparam TFactory The type of descriptor set factory to create and add
     * @tparam ...TArgs Types of arguments to the factory's constructor
     * @param i The index of the descriptor set to replace
     * @param ...args Arguments to the factory's constructor
     * @return A reference to this object
     */
    template<typename TFactory, typename... TArgs>
    ComputePipelineParameters& replaceDescriptorSet(unsigned int i, TArgs&&... args) {
        layoutParams.replaceDescriptorSet<TFactory>(i, std::forward<TArgs>(args)...);
        return *this;
    }

    /**
     * @brief Removes the descriptor set factory at the given index
     *
     * @param i The index of the descriptor set to remove
     * @return A reference to this object
     */
    ComputePipelineParameters& removeDescriptorSet(unsigned int i);

    /**
     * @brief Adds a push constant range config to this pipeline
     *
     * @param offset The offset to place the push constant at
     * @param size The size of the push constant
     * @param shaderStages The shader stages to expose the constant to
     * @return ComputePipelineParameters& A reference to this object
     */
    ComputePipelineParameters& addPushConstantRange(std::uint32_t offset, std::uint32_t size);

private:
    ShaderParameters shader;
    PipelineLayout::LayoutParams layoutParams;
    std::vector<char> shaderConstantStorage;
    std::vector<VkSpecializationMapEntry> shaderConstants;

    friend class ComputePipeline;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif
