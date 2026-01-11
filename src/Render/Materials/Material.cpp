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
                   const res::TextureRef& normal, const res::TextureRef& parallax,
                   float heightScale, float shininess)
: texture(diffuse)
, specularMap(specular)
, normalMap(normal)
, parallaxMap(parallax)
, shininess(shininess)
, heightScale(heightScale) {}

} // namespace mat
} // namespace rc
} // namespace bl
