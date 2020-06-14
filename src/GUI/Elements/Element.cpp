#include <BLIB/GUI/Elements/Element.hpp>

namespace bl
{
namespace gui
{
Element::Element(const std::string& g, const std::string& i)
: _id(i)
, _group(g)
, _dirty(true)
, _active(true)
, _visible(true)
, skipPack(false)
, fillX(false)
, fillY(false)
, isFocused(false)
, focusForced(false)
, isMouseOver(false)
, isLeftPressed(false)
, isRightPressed(false) {}

const std::string& Element::id() const { return _id; }

const std::string& Element::group() const { return _group; }

Element::CPtr Element::getParent() const { return parent.expired() ? nullptr : parent.lock(); }

void Element::setRequisition(const sf::Vector2i& size) {
    requisition.reset();
    if (size.x > 0 && size.y > 0) {
        requisition = size;
        makeDirty();
    }
}

sf::Vector2i Element::getRequisition() const {
    const sf::Vector2i mr = minimumRequisition();
    if (requisition.has_value())
        return {std::max(requisition.value().x, mr.x), std::max(requisition.value().y, mr.y)};
    return mr;
}

const sf::IntRect& Element::getAcquisition() const { return acquisition; }

Signal& Element::getSignal(Action::Type trigger) { return signals[trigger]; }

bool Element::hasFocus() const { return isFocused; }

bool Element::takeFocus() {
    if (isFocused) return true;
    if (!clearFocus()) return false;
    isFocused = true;
    fireSignal(Action(Action::GainedFocus));
    moveToTop();
    return true;
}

bool Element::setForceFocus(bool force) {
    focusForced = force ? takeFocus() : false;
    return hasFocus() || !force;
}

bool Element::clearFocus() {
    WPtr top = parent;
    while (!top.expired()) {
        Ptr t = top.lock();
        if (t) {
            if (t->parent.expired()) break;
            top = t->parent;
        }
        else
            break;
    }
    if (!top.expired()) {
        Ptr t = top.lock();
        if (t) return t->releaseFocus();
    }
    return releaseFocus();
}

bool Element::releaseFocus() {
    if (focusForced) {
        if (hasFocus()) return false;
    }
    bool wasFocused = isFocused;
    isFocused       = false;
    if (wasFocused) fireSignal(Action(Action::LostFocus));
    return true;
}

void Element::moveToTop() const {
    if (!parent.expired()) {
        Element::Ptr p = parent.lock();
        if (p) p->bringToTop(this);
    }
}

bool Element::mouseOver() const { return isMouseOver; }

bool Element::leftPressed() const { return isLeftPressed; }

bool Element::rightPressed() const { return isRightPressed; }

bool Element::handleEvent(const RawEvent& event) {
    if (handleRawEvent(event)) return true;

    const bool eventOnMe =
        acquisition.contains(sf::Vector2i(event.localMousePos.x, event.localMousePos.y));

    switch (event.event.type) {
    case sf::Event::TextEntered:
    case sf::Event::KeyPressed:
    case sf::Event::KeyReleased:
    case sf::Event::MouseWheelScrolled:
        return processAction(Action::fromRaw(event));

    case sf::Event::MouseButtonPressed:
        if (eventOnMe) {
            if (!active()) return true;

            if (takeFocus()) {
                if (event.event.mouseButton.button == sf::Mouse::Left) {
                    dragStart     = event.localMousePos;
                    isLeftPressed = true;
                    return processAction(Action(Action::Pressed, event.localMousePos));
                }
                else if (event.event.mouseButton.button == sf::Mouse::Right) {
                    isRightPressed = true;
                    return true;
                }
            }
            return true; // click blocked
        }
        return false;

    case sf::Event::MouseButtonReleased:
        if (!active()) {
            isLeftPressed = isRightPressed = false;
            return eventOnMe;
        }
        if (event.event.mouseButton.button == sf::Mouse::Left) {
            if (isLeftPressed) {
                isLeftPressed = false;
                processAction(Action(Action::Released, event.localMousePos));
                if (eventOnMe) {
                    processAction(Action(Action::LeftClicked, event.localMousePos));
                    return true;
                }
            }
        }
        else if (event.event.mouseButton.button == sf::Mouse::Right) {
            if (isRightPressed) {
                isRightPressed = false;
                if (eventOnMe) {
                    processAction(Action(Action::RightClicked, event.localMousePos));
                    return true;
                }
            }
        }
        return false;

    case sf::Event::MouseMoved:
        if (!active()) { return eventOnMe; }
        if (isLeftPressed) {
            isMouseOver = eventOnMe;
            const bool r =
                processAction(Action(Action::Dragged, dragStart, event.localMousePos));
            return r;
        }
        else if (eventOnMe) {
            if (!isMouseOver) {
                isMouseOver = true;
                processAction(Action(Action::MouseEntered, event.localMousePos));
            }
            return true;
        }
        else if (isMouseOver) {
            isMouseOver = false;
            processAction(Action(Action::MouseLeft, event.localMousePos));
        }
        return false;

    default:
        return false;
    }
}

bool Element::processAction(const Action& action) {
    if (hasFocus() ||
        (action.type == Action::Scrolled && mouseOver())) { // TODO - other exceptions
        fireSignal(action);
        return true;
    }
    return false;
}

void Element::fireSignal(const Action& action) { signals[action.type](action, this); }

void Element::remove() {
    if (!parent.expired()) {
        Element::Ptr p = parent.lock();
        if (p) p->removeChild(this);
    }
}

void Element::makeDirty() {
    _dirty = true;
    if (!parent.expired()) {
        Element::Ptr p = parent.lock();
        if (p) p->makeDirty();
    }
}

void Element::markClean() { _dirty = false; }

void Element::setVisible(bool v) {
    if (v != _visible) makeDirty();
    _visible = v;
}

bool Element::packable() const {
    if (!visible() || skipPack) return false;
    return shouldPack();
}

void Element::skipPacking(bool s) { skipPack = s; }

bool Element::shouldPack() const { return true; }

bool Element::visible() const { return _visible; }

void Element::setActive(bool a) { _active = a; }

bool Element::active() const { return _active && _visible; }

bool Element::dirty() const { return _dirty; }

void Element::setExpandsWidth(bool expand) {
    if (expand != fillX) makeDirty();
    fillX = expand;
}

bool Element::expandsWidth() const { return fillX; }

void Element::setExpandsHeight(bool expand) {
    if (expand != fillY) makeDirty();
    fillY = expand;
}

bool Element::expandsHeight() const { return fillY; }

void Element::assignAcquisition(const sf::IntRect& acq) {
    markClean();
    acquisition = acq;
    fireSignal(Action(Action::AcquisitionChanged));
}

void Element::setPosition(const sf::Vector2i& pos) {
    const sf::Vector2f diff =
        static_cast<sf::Vector2f>(pos) - sf::Vector2f(acquisition.left, acquisition.top);
    dragStart += diff;
    acquisition.left = pos.x;
    acquisition.top  = pos.y;
}

void Element::setChildParent(Element::Ptr child) { child->parent = me(); }

void Element::render(sf::RenderTarget& target, sf::RenderStates states,
                     const Renderer& renderer) const {
    if (visible()) doRender(target, states, renderer);
}

void Element::doRender(sf::RenderTarget& target, sf::RenderStates states,
                       const Renderer& renderer) const {
    renderer.renderCustom(target, states, *this);
}

const RenderSettings& Element::renderSettings() const { return settings; }

void Element::setFont(bl::Resource<sf::Font>::Ref f) {
    settings.font = f;
    fireSignal(Action(Action::RenderSettingsChanged));
}

void Element::setCharacterSize(unsigned int s) {
    settings.characterSize = s;
    fireSignal(Action(Action::RenderSettingsChanged));
}

void Element::setColor(sf::Color fill, sf::Color outline) {
    settings.fillColor    = fill;
    settings.outlineColor = outline;
    fireSignal(Action(Action::RenderSettingsChanged));
}

void Element::setOutlineThickness(unsigned int t) {
    settings.outlineThickness = t;
    fireSignal(Action(Action::RenderSettingsChanged));
}

void Element::setSecondaryColor(sf::Color fill, sf::Color outline) {
    settings.secondaryFillColor    = fill;
    settings.secondaryOutlineColor = outline;
    fireSignal(Action(Action::RenderSettingsChanged));
}

void Element::setSecondaryOutlineThickness(unsigned int t) {
    settings.secondaryOutlineThickness = t;
    fireSignal(Action(Action::RenderSettingsChanged));
}

void Element::setStyle(sf::Uint32 style) {
    settings.style = style;
    fireSignal(Action(Action::RenderSettingsChanged));
}

void Element::setHorizontalAlignment(RenderSettings::Alignment align) {
    settings.horizontalAlignment = align;
    fireSignal(Action(Action::RenderSettingsChanged));
}

void Element::setVerticalAlignment(RenderSettings::Alignment align) {
    settings.verticalAlignment = align;
    fireSignal(Action(Action::RenderSettingsChanged));
}

Element::Ptr Element::me() { return shared_from_this(); }

} // namespace gui
} // namespace bl