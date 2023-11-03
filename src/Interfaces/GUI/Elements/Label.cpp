#include <BLIB/Interfaces/GUI/Elements/Label.hpp>

#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>
#include <Interfaces/GUI/Data/Font.hpp>

namespace bl
{
namespace gui
{
Label::Ptr Label::create(const std::string& text) { return Ptr(new Label(text)); }

Label::Label(const std::string& text)
: Element()
, text(text) {
    getSignal(Event::RenderSettingsChanged)
        .willAlwaysCall(std::bind(&Label::settingsChanged, this));
}

void Label::setText(const std::string& t) {
    text = t;
    if (getComponent()) { getComponent()->onElementUpdated(); }
    settingsChanged();
}

const std::string& Label::getText() const { return text; }

rdr::Component* Label::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<Label>(
        *this, getParentComponent(), getWindowOrGuiParentComponent());
}

sf::Vector2f Label::minimumRequisition() const {
    if (getComponent()) { return getComponent()->getRequisition(); }
    return {60.f, 15.f};
}

void Label::settingsChanged() {
    const sf::Vector2f req   = getRequisition();
    const sf::FloatRect& acq = getAcquisition();
    if (req.x > acq.width || req.y > acq.height) { makeDirty(); }
}

} // namespace gui
} // namespace bl
