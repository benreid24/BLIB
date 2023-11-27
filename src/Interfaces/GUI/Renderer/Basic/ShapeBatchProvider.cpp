#include <BLIB/Interfaces/GUI/Renderer/Basic/ShapeBatchProvider.hpp>

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
ShapeBatchProvider::ShapeBatchProvider(HighlightState highlightState)
: Component(highlightState)
, enabled(true) {}

gfx::BatchedShapes2D& ShapeBatchProvider::getShapeBatch() { return batch; }

ShapeBatchProvider* defcoms::ShapeBatchProvider::findProvider(rdr::Component* component) {
    while (component != nullptr) {
        ShapeBatchProvider* p = dynamic_cast<ShapeBatchProvider*>(component);
        if (p && p->isEnabled()) { return p; }
        component = component->getParent();
    }
    return nullptr;
}

glm::vec2 ShapeBatchProvider::determineOffset(rdr::Component* component) {
    ShapeBatchProvider* parent = findProvider(component);
    if (!parent) { return {}; }
    const sf::Vector2f pos =
        component->getElement().getPosition() - parent->getElement().getPosition();
    return {pos.x, pos.y};
}

void defcoms::ShapeBatchProvider::setEnabled(bool e) { enabled = e; }

} // namespace defcoms
} // namespace gui
} // namespace bl
