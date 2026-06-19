#include <BLIB/Assets/Builtin/ModelPayload.hpp>

namespace bl
{
namespace asi
{
ModelPayload::ModelPayload(const as::Payload::ConstructContext& ctx)
: as::Payload(ctx)
, nodes()
, meshes()
, bones()
, materials(ctx.repo, *this, "material")
, animations(ctx.repo, *this, "animation") {}

void ModelPayload::flipUVs() { meshes.flipUVs(); }

} // namespace asi
} // namespace bl
