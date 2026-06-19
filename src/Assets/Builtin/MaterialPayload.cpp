#include <BLIB/Assets/Builtin/MaterialPayload.hpp>

namespace bl
{
namespace asi
{
MaterialPayload::CreateData::CreateData(const mdl::Material& material)
: material(material) {}

MaterialPayload::MaterialPayload(const Payload::ConstructContext& ctx)
: Payload(ctx)
, diffuse(ctx.repo, *this, "diffuse")
, normal(ctx.repo, *this, "normal")
, specular(ctx.repo, *this, "specular")
, parallax(ctx.repo, *this, "parallax")
, shininess(0.5f)
, heightScale(0.f) {}

} // namespace asi
} // namespace bl
