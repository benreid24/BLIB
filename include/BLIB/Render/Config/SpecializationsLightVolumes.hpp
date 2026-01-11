#ifndef BLIB_RENDER_CONFIG_SPECIALIZATIONSLIGHTVOLUMES_HPP
#define BLIB_RENDER_CONFIG_SPECIALIZATIONSLIGHTVOLUMES_HPP

#include <cstdint>

namespace bl
{
namespace rc
{
namespace cfg
{
/**
 * @brief Specialization ids for light volume shaders
 *
 * @ingroup Renderer
 */
struct SpecializationsLightVolumes {
    static constexpr std::uint32_t Sunlight         = 0;
    static constexpr std::uint32_t SpotlightShadow  = 1;
    static constexpr std::uint32_t Spotlight        = 2;
    static constexpr std::uint32_t PointlightShadow = 3;
    static constexpr std::uint32_t Pointlight       = 4;
};

} // namespace cfg
} // namespace rc
} // namespace bl

#endif
