#include <BLIB/Render/Resources/MaterialRef.hpp>

#include <BLIB/Render/Resources/MaterialPool.hpp>

namespace bl
{
namespace rc
{
namespace res
{
MaterialRef::MaterialRef(MaterialPool* owner, std::uint32_t id)
: owner(owner)
, id(id) {
    increment();
}

const mat::Material& MaterialRef::operator*() const { return owner->materials[id]; }

const mat::Material* MaterialRef::operator->() const { return &owner->materials[id]; }

mat::Material& MaterialRef::getForEdit() {
    owner->markForUpdate(id);
    return owner->materials[id];
}

void MaterialRef::increment() { ++owner->refCounts[id]; }

void MaterialRef::decrement() {
    if (owner && --owner->refCounts[id] == 0) { owner->release(id); }
}

} // namespace res
} // namespace rc
} // namespace bl
