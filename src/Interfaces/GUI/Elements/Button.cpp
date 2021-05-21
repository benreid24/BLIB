#include <BLIB/Interfaces/GUI/Elements/Button.hpp>

#include <BLIB/Interfaces/GUI/Elements/Label.hpp>
#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>
#include <Interfaces/GUI/Data/Font.hpp>

namespace bl
{
namespace gui
{
Button::Ptr Button::create(const std::string& text, const std::string& group,
                           const std::string& id) {
    Ptr button(new Button(Label::create(text, group, id + "-label"), group, id));
    button->addChild();
    return button;
}

Button::Ptr Button::create(Element::Ptr e, const std::string& g, const std::string& i) {
    Ptr button(new Button(e, g, i));
    button->addChild();
    return button;
}

Button::Button(Element::Ptr child, const std::string& group, const std::string& id)
: Container(group, id)
, child(child) {}

Element::Ptr Button::getChild() const { return child; }

void Button::addChild() { add(child); }

sf::Vector2i Button::minimumRequisition() const {
    sf::Vector2i area = child->getRequisition();
    const int buffer  = renderSettings().outlineThickness.value_or(DefaultOutlineThickness) + 2;
    area.x += buffer * 2;
    area.y += buffer * 2;
    return area;
}

void Button::onAcquisition() {
    Packer::manuallyPackElement(child, {0, 0, getAcquisition().width, getAcquisition().height});
}

bool Button::handleRawEvent(const RawEvent& event) {
    child->handleEvent(transformEvent(event));
    return false; // so the button can have proper mouseover and click events
}

void Button::doRender(sf::RenderTarget& target, sf::RenderStates states,
                      const Renderer& renderer) const {
    renderer.renderButton(target, states, *this);
    renderChildren(target, states, renderer);
    renderer.renderMouseoverOverlay(target, states, this);
}

} // namespace gui
} // namespace bl