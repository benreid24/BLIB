#include <BLIB/Assets/Builtin/Animation2DPayload.hpp>

namespace bl
{
namespace asi
{
Animation2DPayload::Animation2DPayload(const as::Payload::ConstructContext& ctx)
: Payload(ctx)
, spritesheet(ctx.repo, *this, "spritesheet")
, frames()
, loop(false)
, centerShards(false) {}

} // namespace asi
} // namespace bl
