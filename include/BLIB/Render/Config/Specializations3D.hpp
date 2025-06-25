#ifndef BLIB_RENDER_CONFIG_SPECIALIZATIONS3D_HPP
#define BLIB_RENDER_CONFIG_SPECIALIZATIONS3D_HPP

namespace bl
{
namespace rc
{
namespace cfg
{
/**
 * @brief Built-in specializations for 3D pipelines
 *
 * @ingroup Renderer
 */
struct Specializations3D {
    static constexpr std::uint32_t LightingEnabled  = 1;
    static constexpr std::uint32_t LightingDisabled = 2;
    static constexpr std::uint32_t OutlineMainPass  = 3;
    static constexpr std::uint32_t OutlineEdgePass  = 4;
};

} // namespace cfg
} // namespace rc
} // namespace bl

#endif
