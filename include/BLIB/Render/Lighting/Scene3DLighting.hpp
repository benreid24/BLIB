#ifndef BLIB_RENDER_LIGHTING_SCENE3DLIGHTING_HPP
#define BLIB_RENDER_LIGHTING_SCENE3DLIGHTING_HPP

#include <BLIB/Render/Lighting/PointLight3D.hpp>
#include <BLIB/Render/Lighting/SpotLight3D.hpp>
#include <BLIB/Render/Lighting/SunLight3D.hpp>

namespace bl
{
namespace rc
{
namespace lgt
{
class Scene3DLighting {
public:
    static constexpr std::uint32_t MaxPointLights = 128;
    static constexpr std::uint32_t MaxSpotLights  = 128;

private:
    SunLight3D sun;
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
