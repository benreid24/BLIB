#include <BLIB/Engine/EngineFlags.hpp>

namespace bl
{
EngineFlags::EngineFlags()
: flags(0) {}

void EngineFlags::setFlag(Flag flag) { flags |= 1 << flag; }

bool EngineFlags::flagSet(Flag flag) { return (flags & (1 << flag)) != 0; }

void EngineFlags::clear() { flags = 0; }

} // namespace bl
