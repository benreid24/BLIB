#include <BLIB/Render/Uniforms/MaterialUniform.hpp>

namespace bl
{
namespace render
{
MaterialUniform::MaterialUniform()
: ambient(0.1f, 0.1f, 0.1f)
, diffuse(0.5f, 0.5f, 0.5f)
, specular(0.7f, 0.7f, 0.7f)
, shininess(0.1f) {
    // TODO
}

} // namespace render
} // namespace bl
