#include <BLIB/Engine/Flags.hpp>

namespace bl
{
namespace engine
{
namespace
{
constexpr std::uint32_t StateFlags =
    Flags::PopState | Flags::_priv_PushState | Flags::_priv_ReplaceState | Flags::Terminate;
} // namespace

Flags::Flags()
: flags(None) {}

void Flags::set(Flag flag) { flags |= flag; }

bool Flags::active(Flag flag) const { return (flags & flag) != 0; }

bool Flags::stateChangeReady() const { return (flags & StateFlags) != 0; }

void Flags::clear() { flags = None; }

} // namespace engine
} // namespace bl
