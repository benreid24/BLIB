#ifndef BLIB_RENDER_VULKAN_PIPELINEPARAMETERS_HPP
#define BLIB_RENDER_VULKAN_PIPELINEPARAMETERS_HPP

#include <BLIB/Containers/StaticVector.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>
#include <BLIB/Render/Events/SettingsChanged.hpp>
#include <BLIB/Render/Vulkan/BlendParameters.hpp>
#include <BLIB/Render/Vulkan/PipelineLayout.hpp>
#include <BLIB/Render/Vulkan/PipelineSpecialization.hpp>
#include <BLIB/Render/Vulkan/ShaderParameters.hpp>
#include <BLIB/Vulkan.hpp>
#include <array>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <type_traits>
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
     * @brief Signature of a function to modify the pipeline parameters dynamically on settings
     *        events. Should return true if the settings were modified
     */
    using DynamicModifier =
        std::function<bool(Renderer&, PipelineParameters&, const event::SettingsChanged&)>;

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
     * @brief Adds a single shader to this pipeline that is only used if sample shading is enabled
     *
     * @param path The resource path of the shader to add during sample shading
     * @param stage The stage of the shader to add
     * @param entrypoint The entrypoint inside the shader to run
     * @return PipelineParameters& A reference to this object
     */
    PipelineParameters& withSampleShader(const std::string& path, VkShaderStageFlagBits stage,
                                         const std::string& entrypoint = "main");

    /**
     * @brief Adds a single shader to this pipeline that is only used if msaa is enabled, msaa
     *        resolve is not being done by the hardware, and sample shading is not enabled
     *
     * @param path The resource path of the shader to add during multi-sampled rendering
     * @param stage The stage of the shader to add
     * @param entrypoint The entrypoint inside the shader to run
     * @return PipelineParameters& A reference to this object
     */
    PipelineParameters& withResolveShader(const std::string& path, VkShaderStageFlagBits stage,
                                          const std::string& entrypoint = "main");

    /**
     * @brief Clears the given shader stage and replaces it with a noop
     *
     * @param stage The stage of the shader to remove
     * @return A reference to this object
     */
    PipelineParameters& removeShader(VkShaderStageFlagBits stage);

    /**
     * @brief Configures which pipeline states are dynamic. Viewport/scissor must be specified here
     *
     * @param states A set of states to be made dynamic in this pipeline
     * @return PipelineParameters& A reference to this object
     */
    PipelineParameters& withDynamicStates(const std::initializer_list<VkDynamicState>& states);

    /**
     * @brief Configures which pipeline states are dynamic. Viewport/scissor are dynamic by default
     *
     * @param states A set of states to be made dynamic in this pipeline
     * @return PipelineParameters& A reference to this object
     */
    PipelineParameters& addDynamicStates(const std::initializer_list<VkDynamicState>& states);

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
     * @brief Enables depth clipping by modifying the rasterizer state. Call after withRasterizer()
     *
     * @return A reference to this object
     */
    PipelineParameters& withEnableDepthClipping();

    /**
     * @brief Configures sample shading for this pipeline. Sample shading is disabled by default
     *
     * @param enabled Whether sample shading is enabled
     * @param minSampleShading The ratio of samples that must be shaded
     * @return A reference to this object
     */
    PipelineParameters& withSampleShading(bool enabled, float minSampleShading = 0.0f);

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
     * @brief Configures the attachment blending of the pipeline
     *
     * @param blendConfig The attachment blend states
     * @return A reference to this object
     */
    PipelineParameters& withBlendConfig(const BlendParameters& blendConfig);

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
     * @param depthTest Whether depth testing is enabled
     * @param depthWrite Whether depth writing is enabled
     * @param stencilTest Whether stencil testing is enabled
     * @param stencilWrite Whether stencil writing is enabled
     * @return A reference to this object
     */
    PipelineParameters& withSimpleDepthStencil(bool depthTest, bool depthWrite = true,
                                               bool stencilTest = false, bool stencilWrite = false);

    /**
     * @brief Declares the number of specializations that will be used in this pipeline
     *
     * @param count The number of specializations to declare
     * @return A reference to this object
     */
    PipelineParameters& withDeclareSpecializations(std::uint32_t count);

    /**
     * @brief Adds a specialization to the pipeline
     *
     * @param specializationId The id of the specialization. Should be an index starting at 1. An id
     *                         of 0 will affect the main pipeline itself
     * @param specialization The specialization parameters
     * @return A reference to this object
     */
    PipelineParameters& withSpecialization(std::uint32_t specializationId,
                                           const PipelineSpecialization& specialization);

    /**
     * @brief Adds a modifier hook that will be called on settings changed events
     *
     * @param TModifier The type of the modifier function to call on settings changed events
     * @param modifier The modifier function to call on settings changed events
     * @return A reference to this object
     */
    template<typename TModifier>
    PipelineParameters& withDynamicModifier(TModifier&& modifier);

    /**
     * @brief Helper function that allows dynamic modifiers to change all specializations
     *
     * @tparam TVisitor The type of the visitor to call for each specialization
     * @param visitor The visitor to call for each specialization. Signature should be:
     *                void(PipelineSpecialization&)
     */
    template<typename TVisitor>
    void visitSpecializations(TVisitor&& visitor);

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
    struct DescriptorSet {
        std::type_index factoryType;
        std::unique_ptr<ds::DescriptorSetFactory> factory;

        DescriptorSet(std::type_index tid, std::unique_ptr<ds::DescriptorSetFactory>&& factory)
        : factoryType(tid)
        , factory(std::forward<std::unique_ptr<ds::DescriptorSetFactory>>(factory)) {}
    };

    PipelineLayout::LayoutParams layoutParams;
    ctr::StaticVector<ShaderParameters, 5> shaders;
    ctr::StaticVector<ShaderParameters, 5> resolveShaders;
    ctr::StaticVector<ShaderParameters, 5> sampleShaders;
    ctr::StaticVector<VkDynamicState, 8> dynamicStates;
    VkVertexInputBindingDescription vertexBinding;
    ctr::StaticVector<VkVertexInputAttributeDescription, 8> vertexAttributes;
    VkPrimitiveTopology primitiveType;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    BlendParameters colorBlending;
    VkPipelineDepthStencilStateCreateInfo* depthStencil;
    VkPipelineMultisampleStateCreateInfo msaa;
    PipelineSpecialization mainSpecialization;
    std::vector<PipelineSpecialization> specializations;
    std::vector<DynamicModifier> dynamicModifiers;

    VkPipelineRasterizationDepthClipStateCreateInfoEXT localDepthClipping;
    VkPipelineDepthStencilStateCreateInfo localDepthStencil;

    bool handleChange(Renderer& renderer, const event::SettingsChanged& changeEvent);

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

template<typename TModifier>
PipelineParameters& PipelineParameters::withDynamicModifier(TModifier&& modifier) {
    dynamicModifiers.emplace_back(std::forward<TModifier>(modifier));
    return *this;
}

template<typename TVisitor>
void PipelineParameters::visitSpecializations(TVisitor&& visitor) {
    static_assert(std::is_invocable_v<TVisitor, PipelineSpecialization&>,
                  "Visitor must be invocable with PipelineSpecialization&");
    visitor(mainSpecialization);
    for (PipelineSpecialization& spec : specializations) { visitor(spec); }
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
