#include <BLIB/Assets/Builtin/CubemapPayload.hpp>

namespace bl
{
namespace asi
{
CubemapPayload::CubemapPayload(const as::Payload::ConstructContext& ctx)
: as::Payload(ctx)
, top(ctx.repo, *this, "top")
, bottom(ctx.repo, *this, "bottom")
, left(ctx.repo, *this, "left")
, right(ctx.repo, *this, "right")
, front(ctx.repo, *this, "front")
, back(ctx.repo, *this, "back") {}

} // namespace asi
} // namespace bl
