#ifndef BLIB_RENDER_CONFIG_MATERIALPIPELINEIDS_HPP
#define BLIB_RENDER_CONFIG_MATERIALPIPELINEIDS_HPP

#include <cstdint>

namespace bl
{
namespace rc
{
namespace cfg
{
/**
 * @brief Built-in MaterialPipeline ids
 *
 * @ingroup Renderer
 */
struct MaterialPipelineIds {
    static constexpr std::uint32_t Mesh3D            = 1;
    static constexpr std::uint32_t Mesh3DMaterial    = 2;
    static constexpr std::uint32_t Mesh3DSkinned     = 3;
    static constexpr std::uint32_t Geometry2D        = 4;
    static constexpr std::uint32_t Geometry2DSkinned = 5;
    static constexpr std::uint32_t Text              = 6;
    static constexpr std::uint32_t Slideshow2D       = 7;
    static constexpr std::uint32_t Lines2D           = 8;
    static constexpr std::uint32_t Skybox            = 9;
};

} // namespace cfg
} // namespace rc
} // namespace bl

#endif
