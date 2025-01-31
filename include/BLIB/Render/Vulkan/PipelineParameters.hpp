#ifndef BLIB_RENDER_VULKAN_PIPELINEPARAMETERS_HPP
#define BLIB_RENDER_VULKAN_PIPELINEPARAMETERS_HPP

#include <BLIB/Containers/StaticVector.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>
#include <BLIB/Render/Vulkan/PipelineLayout.hpp>
#include <BLIB/Vulkan.hpp>
#include <array>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <typeindex>
#include <vector>

namespace bl
{
namespace rc
{
namespace vk
{
class Pipeline;

/**
 * @brief Parameters used to create a Pipeline. This class performs some basic validation and
 *        provides sane defaults
 *
 * @ingroup Renderer
 */
class PipelineParameters {
public:
    /**
     * @brief Creates the parameters using sane defaults
     */
    PipelineParameters();

    /**
     * @brief Copies the pipeline parameters
     *
     * @param copy The parameters to copy
     */
    PipelineParameters(const PipelineParameters& copy);

    /**
     * @brief Helper method to setup vertex and fragment shaders for the pipeline
     *
     * @param vertexPath The resource path of the vertex shader
     * @param fragPath The resource path of the fragment shader
     * @return PipelineParameters& A reference to this object
     */
    PipelineParameters& withShaders(const std::string& vertexPath, const std::string& fragPath);

    /**
     * @brief Adds a single shader to this pipeline
     *
     * @param path The resource path of the shader to add
     * @param stage The stage of the shader to add
     * @param entrypoint The entrypoint inside the shader to run
     * @return PipelineParameters& A reference to this object
     */
    PipelineParameters& withShader(const std::string& path, VkShaderStageFlagBits stage,
                                   const std::string& entrypoint = "main");

    /**
     * @brief Configures which pipeline states are dynamic. Viewport and scissor are always dynamic
     *
     * @param states A set of states to be made dynamic in this pipeline
     * @return PipelineParameters& A reference to this object
     */
    PipelineParameters& withDynamicStates(const std::initializer_list<VkDynamicState>& states);

    /**
     * @brief Configures the vertex format and attributes for the vertices that will be rendered by
     *        this pipeline. Defaults to Vertex if not specified
     *
     * @tparam N The number of vertex attributes
     * @param binding The vertex binding description
     * @param attributes The vertex attributes
     * @return PipelineParameters& A reference to this object
     */
    template<std::size_t N>
    PipelineParameters& withVertexFormat(
        const VkVertexInputBindingDescription& binding,
        const std::array<VkVertexInputAttributeDescription, N>& attributes);

    /**
     * @brief Sets the primitive type that the vertices within this pipeline represent. Defaults to
     *        triangle list
     *
     * @param primitiveType The primitive type to be rendered
     * @return PipelineParameters& A reference to this object
     */
    PipelineParameters& withPrimitiveType(VkPrimitiveTopology primitiveType);

    /**
     * @brief Configures the rasterizer for this pipeline. Defaults to sane values
     *
     * @param rasterizer The rasterizer config
     * @return PipelineParameters& A reference to this object
     */
    PipelineParameters& withRasterizer(const VkPipelineRasterizationStateCreateInfo& rasterizer);

    /**
     * @brief Configures MSAA for this pipeline. MSAA is disabled by default
     *
     * @param msaaParams The MSAA parameters
     * @return PipelineParameters& A reference to this object
     */
    PipelineParameters& withMSAA(const VkPipelineMultisampleStateCreateInfo& msaaParams);

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
    PipelineParameters& addDescriptorSet(TArgs&&... args);

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
    PipelineParameters& replaceDescriptorSet(unsigned int i, TArgs&&... args);

    /**
     * @brief Removes the descriptor set factory at the given index
     *
     * @param i The index of the descriptor set to remove
     * @return A reference to this object
     */
    PipelineParameters& removeDescriptorSet(unsigned int i);

    /**
     * @brief Adds a push constant range config to this pipeline. All pipelines are configured to
     *        accept PushConstants at the beginning of the range. This may be called to allow custom
     *        push constants to be used in addition to the defaults.
     *
     * @param offset The offset to place the push constant at
     * @param size The size of the push constant
     * @param shaderStages The shader stages to expose the constant to
     * @return PipelineParameters& A reference to this object
     */
    PipelineParameters& addPushConstantRange(
        std::uint32_t offset, std::uint32_t size,
        VkShaderStageFlags shaderStages = VK_SHADER_STAGE_ALL_GRAPHICS);

    /**
     * @brief Adds an alpha blending config for a color attachment for this pipeline. Blend configs
     *        are ordered by the order that this method is called in. Defaults to a single blend
     *        config with standard alpha blending if not overridden.
     *
     * @param blendState The blend config to add
     * @return PipelineParameters& A reference to this object
     */
    PipelineParameters& addColorAttachmentBlendState(
        const VkPipelineColorBlendAttachmentState& blendState);

    /**
     * @brief Overrides the blend configuration for all color attachments for this pipeline.
     *        Defaults to disabled
     *
     * @param operation The blend operation to use
     * @param blendConstant0 The R blend constant
     * @param blendConstant1 The G blend constant
     * @param blendConstant2 The B blend constant
     * @param blendConstant3 The A blend constant
     * @return PipelineParameters& A reference to this object
     */
    PipelineParameters& withColorBlendStateConfig(VkLogicOp operation, float blendConstant0,
                                                  float blendConstant1, float blendConstant2,
                                                  float blendConstant3);

    /**
     * @brief Configures depth and stencil testing for this pipeline
     *
     * @param depthStencil Pointer to the depth/stencil config. Must remain valid until created
     * @return A reference to this object
     */
    PipelineParameters& withDepthStencilState(VkPipelineDepthStencilStateCreateInfo* depthStencil);

    /**
     * @brief Helper function to populate the depth stencil state based on depth testing setting
     *
     * @param depthTestingEnabled Whether or not to enable depth testing
     * @return A reference to this object
     */
    PipelineParameters& withSimpleDepthStencil(bool depthTestingEnabled);

    /**
     * @brief Performs final validation and defaulting, then returns an rvalue reference to this
     *        object to be used for pipeline creation
     *
     * @return PipelineParameters&& An rvalue reference to this object
     */
    PipelineParameters&& build();

    /**
     * @brief Tests whether these parameters are equal to the given parameters
     *
     * @param right The parameters to compare to
     * @return True if the parameters are equal, false otherwise
     */
    bool operator==(const PipelineParameters& right) const;

    /**
     * @brief Tests whether these parameters are not equal to the given parameters
     *
     * @param right The parameters to compare to
     * @return True if the parameters are not equal, false otherwise
     */
    bool operator!=(const PipelineParameters& right) const;

private:
    struct ShaderInfo {
        std::string path;
        VkShaderStageFlagBits stage;
        std::string entrypoint;

        ShaderInfo() = default;
        ShaderInfo(const std::string& p, VkShaderStageFlagBits s, const std::string& e)
        : path(p)
        , stage(s)
        , entrypoint(e) {}
    };

    struct DescriptorSet {
        std::type_index factoryType;
        std::unique_ptr<ds::DescriptorSetFactory> factory;

        DescriptorSet(std::type_index tid, std::unique_ptr<ds::DescriptorSetFactory>&& factory)
        : factoryType(tid)
        , factory(std::forward<std::unique_ptr<ds::DescriptorSetFactory>>(factory)) {}
    };

    PipelineLayout::LayoutParams layoutParams;
    ctr::StaticVector<ShaderInfo, 5> shaders;
    ctr::StaticVector<VkDynamicState, 8> dynamicStates;
    VkVertexInputBindingDescription vertexBinding;
    ctr::StaticVector<VkVertexInputAttributeDescription, 6> vertexAttributes;
    VkPrimitiveTopology primitiveType;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    VkPipelineMultisampleStateCreateInfo msaa;
    ctr::StaticVector<VkPipelineColorBlendAttachmentState, 4> colorAttachmentBlendStates;
    VkPipelineColorBlendStateCreateInfo colorBlending;
    VkPipelineDepthStencilStateCreateInfo* depthStencil;

    VkPipelineDepthStencilStateCreateInfo localDepthStencil;

    friend class Pipeline;
    friend struct std::hash<PipelineParameters>;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<std::size_t N>
PipelineParameters& PipelineParameters::withVertexFormat(
    const VkVertexInputBindingDescription& binding,
    const std::array<VkVertexInputAttributeDescription, N>& attributes) {
    vertexBinding = binding;
    vertexAttributes.resize(N);
    std::copy(attributes.begin(), attributes.end(), vertexAttributes.begin());
    return *this;
}

template<typename T, typename... TArgs>
PipelineParameters& PipelineParameters::addDescriptorSet(TArgs&&... args) {
    layoutParams.addDescriptorSet<T>(std::forward<TArgs>(args)...);
    return *this;
}

template<typename TFactory, typename... TArgs>
PipelineParameters& PipelineParameters::replaceDescriptorSet(unsigned int i, TArgs&&... args) {
    layoutParams.replaceDescriptorSet<TFactory>(i, std::forward<TArgs>(args)...);
    return *this;
}

} // namespace vk
} // namespace rc
} // namespace bl

namespace std
{
template<>
struct hash<bl::rc::vk::PipelineParameters> {
    std::size_t operator()(const bl::rc::vk::PipelineParameters& params) const;
};
} // namespace std

#endif
