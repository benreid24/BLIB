#ifndef BLIB_RENDER_CONFIG_COMPUTEPIPELINEIDS_HPP
#define BLIB_RENDER_CONFIG_COMPUTEPIPELINEIDS_HPP

#include <cstdint>

namespace bl
{
namespace rc
{
namespace cfg
{
/**
 * @brief Built-in compute pipeline IDs
 *
 * @ingroup Renderer
 */
struct ComputePipelineIds {
    static constexpr std::uint32_t AutoExposureAccumulate = 1;
    static constexpr std::uint32_t AutoExposureAdjust     = 2;
};

} // namespace cfg
} // namespace rc
} // namespace bl

#endif
