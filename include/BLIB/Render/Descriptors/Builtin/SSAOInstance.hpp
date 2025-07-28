#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_SSAOINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_SSAOINSTANCE_HPP

#include <BLIB/Render/Buffers/StaticUniformBuffer.hpp>
#include <BLIB/Render/Descriptors/Generic/GlobalUniformBuffer.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetInstance.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Uniform payload for the SSAO algorithm
 *
 * @ingroup Renderer
 */
struct SSAOShaderPayload {
    glm::vec4 randomRotations[4][4];
    glm::vec4 samples[128];
    std::uint32_t sampleCount;
    float radius;
    float bias;
    float exponent;
};

/**
 * @brief The bindings for the SSAO descriptor set
 *
 * @ingroup Renderer
 */
using SSAOBindings = Bindings<GlobalUniformBuffer<SSAOShaderPayload>>;

/**
 * @brief The descriptor set containing the SSAO parameters
 *
 * @ingroup Renderer
 */
using SSAOInstance = GenericDescriptorSetInstance<SSAOBindings>;

} // namespace ds
} // namespace rc
} // namespace bl

#endif
