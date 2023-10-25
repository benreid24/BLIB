#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>

namespace bl
{
namespace gui
{
namespace rdr
{
Component::Component()
: renderer(nullptr)
, owner(nullptr)
, state(UIState::Regular) {}

void Component::showTooltip(const std::string& text, const glm::vec2& pos) {
    // TODO - call into renderer?
}

void Component::create(engine::Engine& engine, Renderer& r, Element& o, Component& windowOrGui) {
    renderer = &r;
    owner    = &o;
    doCreate(engine, r, windowOrGui);
    if (o.active()) {
        if (o.rightPressed() || o.leftPressed()) { setUIState(UIState::Pressed); }
        else if (o.mouseOver()) { setUIState(UIState::Highlighted); }
        else { setUIState(UIState::Regular); }
    }
    else { setUIState(UIState::Disabled); }
}

} // namespace rdr
} // namespace gui
} // namespace bl
