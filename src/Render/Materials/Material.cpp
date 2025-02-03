#include <BLIB/Render/Materials/Material.hpp>

namespace bl
{
namespace rc
{
namespace mat
{
Material::Material() {}

Material::Material(const res::TextureRef& texture)
: texture(texture) {}

} // namespace mat
} // namespace rc
} // namespace bl
