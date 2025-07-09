#ifndef BLIB_RENDER_VULKAN_BLENDPARAMETERS_HPP
#define BLIB_RENDER_VULKAN_BLENDPARAMETERS_HPP

#include <BLIB/Containers/StaticVector.hpp>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
class Pipeline;
class PipelineParameters;

class BlendParameters {
public:
    /// Defines the basic color blend behaviors if no specific blend state is provided. Default is
    /// AlphaBlend
    enum struct ColorBlendBehavior { None, AlphaBlend, Overwrite, Add };

    /**
     * @brief Creates the parameters with the default alpha blend state
     */
    BlendParameters();

    /**
     * @brief Basic copy constructor
     */
    BlendParameters(const BlendParameters& copy);

    /**
     * @brief Basic assignment operator
     */
    BlendParameters& operator=(const BlendParameters& copy);

    /**
     * @brief Adds an alpha blending config for a color attachment for this pipeline. Blend configs
     *        are ordered by the order that this method is called in. Defaults to a single blend
     *        config with standard alpha blending if not overridden.
     *
     * @param blendState The blend config to add
     * @return PipelineParameters& A reference to this object
     */
    BlendParameters& addColorAttachmentBlendState(
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
    BlendParameters& withColorBlendStateConfig(VkLogicOp operation, float blendConstant0,
                                               float blendConstant1, float blendConstant2,
                                               float blendConstant3);

    /**
     * @brief Simple way to set common color blend behaviors. Ignored if color blend attachments are
     *        manually set
     *
     * @param blendState The color blend state to use
     * @param attachmentCount The number of color attachments to apply this blend state to
     * @return A reference to this object
     */
    BlendParameters& withSimpleColorBlendState(ColorBlendBehavior blendState,
                                               std::uint32_t attachmentCount = 1);

    /**
     * @brief Tests equality between two BlendParameters objects
     *
     * @param right The parameters to compare to
     * @return True if the parameters are equal, false otherwise
     */
    bool operator==(const BlendParameters& right) const;

private:
    ColorBlendBehavior colorBlendBehavior;
    ctr::StaticVector<VkPipelineColorBlendAttachmentState, 4> colorAttachmentBlendStates;
    VkPipelineColorBlendStateCreateInfo colorBlending;
    std::uint32_t simpleColorBlendAttachmentCount;

    void build();

    friend class Pipeline;
    friend class PipelineParameters;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif
