#include <BLIB/Engine/Flags.hpp>

namespace bl
{
namespace engine
{
Flags::Flags()
: flags(0) {}

void Flags::set(Flag flag) { flags |= 1 << flag; }

bool Flags::active(Flag flag) { return (flags & (1 << flag)) != 0; }

void Flags::clear() { flags = 0; }

} // namespace engine
} // namespace bl
