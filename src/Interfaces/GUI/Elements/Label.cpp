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
, text(text)
, wrapBehavior(WrapDisabled) {
    getSignal(Event::RenderSettingsChanged)
        .willAlwaysCall(std::bind(&Label::settingsChanged, this));
}

void Label::setText(const std::string& t) {
    text = t;
    if (getComponent()) { getComponent()->onElementUpdated(); }
    settingsChanged();
}

const std::string& Label::getText() const { return text; }

void Label::setTextWrapping(TextWrapBehavior behavior, float width) {
    wrapBehavior = behavior;
    wrapWidth    = width;

    if (wrapBehavior == WrapFixedWidth && width < 0.f) {
        BL_LOG_WARN << "Width must be specified when wrapping to fixed width";
        wrapWidth = 100.f;
    }

    if (getComponent()) { getComponent()->onElementUpdated(); }
    settingsChanged();
}

Label::TextWrapBehavior Label::getTextWrapBehavior() const { return wrapBehavior; }

float Label::getTextWrapWidth() const { return wrapWidth; }

rdr::Component* Label::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<Label>(*this);
}

sf::Vector2f Label::minimumRequisition() const {
    if (getComponent()) { return getComponent()->getRequisition(); }
    return {60.f, 15.f};
}

void Label::settingsChanged() {
const sf::Vector2f req   = getRequisition();
const sf::FloatRect& acq = getAcquisition();
if (req.x != acq.size.x || req.y != acq.size.y) { makeDirty(); }
}

void Label::update(float dt) {
    Element::update(dt);

    if (wrapBehavior == TextWrapBehavior::WrapToAcquisition && getComponent()) {
        if (getAcquisition().size.x > 0.f &&
            getComponent()->getRequisition().x != getAcquisition().size.x) {
            makeDirty();
        }
    }
}

} // namespace gui
} // namespace bl
