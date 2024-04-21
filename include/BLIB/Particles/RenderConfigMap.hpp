#ifndef BLIB_PARTICLES_RENDERCONFIGMAP_HPP
#define BLIB_PARTICLES_RENDERCONFIGMAP_HPP

#include <cstdint>
#include <limits>

namespace bl
{
namespace pcl
{
namespace priv
{
constexpr std::uint32_t PipelineNotSet = std::numeric_limits<std::uint32_t>::max();
}

/**
 * @brief Compile-time mapping struct for use with the default Renderer<T> implementation.
 *        Specialize this for your particle type and provide the pipeline and transparency settings
 *        to render with
 *
 * @tparam T The particle type to specify render settings for
 * @ingroup Particles
 */
template<typename T>
struct RenderConfigMap {
    /// The id of the pipeline to render the particle system with
    static constexpr std::uint32_t PipelineId = priv::PipelineNotSet;

    /// The transparency setting to render the particle system with
    static constexpr bool ContainsTransparency = false;
};

} // namespace pcl
} // namespace bl

#endif
