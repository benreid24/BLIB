#ifndef BLIB_RENDER_VULKAN_PIPELINEPARAMETERS_HPP
#define BLIB_RENDER_VULKAN_PIPELINEPARAMETERS_HPP

#include <array>
#include <cstdint>
#include <functional>
#include <glad/vulkan.h>
#include <initializer_list>
#include <string>
#include <vector>

namespace bl
{
namespace render
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
    /// @brief Callback used to retrieve a descriptor set at pipeline bind time
    using DescriptorSetRetriever = std::function<VkDescriptorSet()>;

    /**
     * @brief Construct new Pipeline Parameters for a given render pass
     *
     * @param renderPassId The id of the render pass the pipeline will be used with
     */
    PipelineParameters(std::uint32_t renderPassId);

    /**
     * @brief Configures which subpass this pipeline is for. Default is 0
     *
     * @param subpassIndex The subpass this pipeline is for
     * @return A reference to this object
     */
    PipelineParameters& forSubpass(std::uint32_t subpassIndex);

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
    PipelineParameters& addShader(const std::string& path, VkShaderStageFlagBits stage,
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
    template<std::uint32_t N>
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
     * @brief Adds a descriptor set layout and retriever to this pipeline. Set numbers start at 0
     * and added sets are numbered by the order this method is called in.
     *
     * @param layout The layout of the descriptor set
     * @param retriever Callback that will be called at pipeline bind time to get the set to bind
     * @return PipelineParameters& A reference to this object
     */
    PipelineParameters& addDescriptorSet(VkDescriptorSetLayout layout,
                                         DescriptorSetRetriever&& retriever);

    /**
     * @brief Adds a descriptor set layout with no retriever. Code rendering with this pipeline will
     *        have to manually bind the descriptor sets
     *
     * @param layout The layout of the descriptor set
     * @param retriever Callback that will be called at pipeline bind time to get the set to bind
     * @return PipelineParameters& A reference to this object
     */
    PipelineParameters& addDescriptorSet(VkDescriptorSetLayout layout);

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
     * @brief Set whether or not batched objects will have their order preserved as objects are
     *        added and removed. Allowing objects to be reordered is more efficient, but may change
     *        renderer output if depth testing is not used. Defaults to false
     *
     * @param preserveOrder True to preserve object orders, false otherwise
     * @return A reference to this object
     */
    PipelineParameters& withPreserveObjectOrder(bool preserveOrder);

    /**
     * @brief Configures depth and stencil testing for this pipeline
     *
     * @param depthStencil Pointer to the depth/stencil config. Must remain valid until created
     * @return A reference to this object
     */
    PipelineParameters& withDepthStencilState(VkPipelineDepthStencilStateCreateInfo* depthStencil);

    /**
     * @brief Performs final validation and defaulting, then returns an rvalue reference to this
     *        object to be used for pipeline creation
     *
     * @return PipelineParameters&& An rvalue reference to this object
     */
    PipelineParameters&& build();

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

    std::vector<ShaderInfo> shaders;
    std::vector<VkDynamicState> dynamicStates;
    VkVertexInputBindingDescription vertexBinding;
    std::vector<VkVertexInputAttributeDescription> vertexAttributes;
    VkPrimitiveTopology primitiveType;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    VkPipelineMultisampleStateCreateInfo msaa;
    std::vector<VkDescriptorSetLayout> descriptorSets;
    std::vector<DescriptorSetRetriever> descriptorSetRetrievers;
    std::vector<VkPushConstantRange> pushConstants;
    std::vector<VkPipelineColorBlendAttachmentState> colorAttachmentBlendStates;
    VkPipelineColorBlendStateCreateInfo colorBlending;
    VkPipelineDepthStencilStateCreateInfo* depthStencil;
    std::uint32_t renderPassId;
    std::uint32_t subpass;
    bool preserveOrder;

    friend class Pipeline;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<std::uint32_t N>
PipelineParameters& PipelineParameters::withVertexFormat(
    const VkVertexInputBindingDescription& binding,
    const std::array<VkVertexInputAttributeDescription, N>& attributes) {
    vertexBinding = binding;
    vertexAttributes.resize(N);
    std::copy(attributes.begin(), attributes.end(), vertexAttributes.begin());
    return *this;
}

} // namespace render
} // namespace bl

#endif
