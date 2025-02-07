#include <BLIB/Render/Materials/Material.hpp>

namespace bl
{
namespace rc
{
namespace mat
{
Material::Material()
: shininess(0.5f) {}

Material::Material(const res::TextureRef& texture)
: texture(texture)
, shininess(0.5f) {}

} // namespace mat
} // namespace rc
} // namespace bl
