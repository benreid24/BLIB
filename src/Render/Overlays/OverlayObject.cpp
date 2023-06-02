#include <BLIB/Render/Overlays/OverlayObject.hpp>

namespace bl
{
namespace render
{
namespace ovy
{
OverlayObject::OverlayObject()
: SceneObject()
, descriptorCount(0) {}

void OverlayObject::registerChild(std::uint32_t cid) {
    if (children.size() == children.capacity()) {
        children.reserve(std::max(children.capacity() * 2, static_cast<std::size_t>(8)));
    }
    children.emplace_back(cid);
}

void OverlayObject::removeChild(std::uint32_t cid) {
    for (auto it = children.begin(); it != children.end(); ++it) {
        if (*it == cid) {
            children.erase(it);
            return;
        }
    }
}

} // namespace ovy
} // namespace render
} // namespace bl
