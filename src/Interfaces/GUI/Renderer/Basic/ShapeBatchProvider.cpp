#include <BLIB/Interfaces/GUI/Renderer/Basic/ShapeBatchProvider.hpp>

#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
ShapeBatchProvider::ShapeBatchProvider()
: enabled(true) {}

gfx::BatchedShapes2D& ShapeBatchProvider::getShapeBatch() { return batch; }

ShapeBatchProvider* defcoms::ShapeBatchProvider::findProvider(rdr::Component* component) {
    while (component != nullptr) {
        ShapeBatchProvider* p = dynamic_cast<ShapeBatchProvider*>(component);
        if (p && p->isEnabled()) { return p; }
        component = component->getParent();
    }
    return nullptr;
}

void defcoms::ShapeBatchProvider::setEnabled(bool e) { enabled = e; }

} // namespace defcoms
} // namespace gui
} // namespace bl
