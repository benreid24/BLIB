#include <BLIB/Render/Scenes/TargetTable.hpp>

#include <limits>

namespace bl
{
namespace rc
{
namespace scene
{
unsigned int TargetTable::addTarget(RenderTarget* target) {
    for (unsigned int i = 0; i < targets.size(); ++i) {
        if (targets[i] == target || !targets[i]) { return i; }
    }

    const unsigned int i = targets.size();
    targets.emplace_back(target);
    return i;
}

unsigned int TargetTable::getTargetIndex(RenderTarget* target) const {
    for (unsigned int i = 0; i < targets.size(); ++i) {
        if (targets[i] == target) { return i; }
    }
    return std::numeric_limits<unsigned int>::max();
}

RenderTarget* TargetTable::getTarget(unsigned int index) const { return targets[index]; }

RenderTarget* TargetTable::removeTarget(unsigned int index) {
    RenderTarget* target = targets[index];
    targets[index]       = nullptr;
    return target;
}

unsigned int TargetTable::nextId() const { return targets.size(); }

} // namespace scene
} // namespace rc
} // namespace bl
