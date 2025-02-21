#include <BLIB/Render/Materials/Material.hpp>

namespace bl
{
namespace rc
{
namespace mat
{
Material::Material()
: shininess(0.5f) {}

Material::Material(const res::TextureRef& diffuse, const res::TextureRef& specular,
                   const res::TextureRef& normal, float shininess)
: texture(diffuse)
, specularMap(specular)
, normalMap(normal)
, shininess(shininess) {}

} // namespace mat
} // namespace rc
} // namespace bl
