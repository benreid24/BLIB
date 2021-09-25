#include <BLIB/Interfaces/GUI/Elements/Button.hpp>

#include <BLIB/Interfaces/GUI/Elements/Label.hpp>
#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>
#include <Interfaces/GUI/Data/Font.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace gui
{
namespace
{
constexpr int ChildPadding = 2;
}

Button::Ptr Button::create(const std::string& text) { return Ptr(new Button(Label::create(text))); }

Button::Ptr Button::create(Element::Ptr e) { return Ptr(new Button(e)); }

Button::Button(Element::Ptr child)
: Element()
, child(child) {
    getSignal(Event::AcquisitionChanged).willAlwaysCall(std::bind(&Button::onAcquisition, this));
}

Element::Ptr Button::getChild() const { return child; }

sf::Vector2i Button::minimumRequisition() const {
    sf::Vector2i area = child->getRequisition();
    const int buffer  = renderSettings().outlineThickness.value_or(DefaultOutlineThickness) + 2;
    area.x += (buffer + ChildPadding) * 2;
    area.y += (buffer + ChildPadding) * 2;
    return area;
}

void Button::onAcquisition() {
    BL_LOG_INFO << "Packing button into (" << getAcquisition().left << ", " << getAcquisition().top
                << ", " << getAcquisition().width << ", " << getAcquisition().height << ")";
    Packer::manuallyPackElement(child,
                                {getAcquisition().left + ChildPadding,
                                 getAcquisition().top + ChildPadding,
                                 getAcquisition().width - ChildPadding * 2,
                                 getAcquisition().height - ChildPadding * 2});
}

bool Button::propagateEvent(const Event& event) {
    child->processEvent(event);
    return false; // so the button can have proper mouseover and click events
}

void Button::doRender(sf::RenderTarget& target, sf::RenderStates states,
                      const Renderer& renderer) const {
    renderer.renderButton(target, states, *this);
    child->render(target, states, renderer);
    renderer.renderMouseoverOverlay(target, states, this);
}

} // namespace gui
} // namespace bl