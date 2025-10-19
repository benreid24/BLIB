#include <BLIB/Render/Buffers/DirtyRange.hpp>

#include <algorithm>

namespace bl
{
namespace rc
{
namespace buf
{
DirtyRange::DirtyRange()
: start(0)
, size(0) {}

DirtyRange::DirtyRange(std::uint32_t i, std::uint32_t n)
: start(i)
, size(n) {}

void DirtyRange::combine(const DirtyRange& other) {
    const std::uint32_t end = std::max(start + size, other.start + other.size);
    start                   = std::min(start, other.start);
    size                    = end - start;
}

void DirtyRange::reset() {
    start = 0;
    size  = 0;
}

void DirtyRange::markDirty(std::uint32_t i, std::uint32_t n) { combine(DirtyRange{i, n}); }

} // namespace buf
} // namespace rc
} // namespace bl
