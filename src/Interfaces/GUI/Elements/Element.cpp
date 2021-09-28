#include <BLIB/Interfaces/GUI/Elements/Element.hpp>

namespace bl
{
namespace gui
{
Element::Element()
: _dirty(true)
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

Signal& Element::getSignal(Event::Type trigger) { return signals[trigger]; }

bool Element::hasFocus() const { return isFocused; }

bool Element::takeFocus() {
    if (isFocused) return true;
    if (!clearFocus()) return false;
    isFocused = true;
    fireSignal(Event(Event::GainedFocus));
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
    if (wasFocused) fireSignal(Event(Event::LostFocus));
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

bool Element::processEvent(const Event& event) {
    if (!visible()) return false;

    if (propagateEvent(event)) return true;

    if (event.type() == Event::Scrolled) {
        if (getAcquisition().contains(sf::Vector2i(event.mousePosition()))) {
            return handleScroll(event);
        }
        return false;
    }

    const bool eventOnMe = acquisition.contains(sf::Vector2i(event.mousePosition()));

    switch (event.type()) {
    case Event::TextEntered:
    case Event::KeyPressed:
    case Event::KeyReleased:
    case Event::Scrolled:
        return processAction(event); // TODO - is this right?

    case Event::LeftMousePressed:
    case Event::RightMousePressed:
        if (eventOnMe) {
            if (!active()) return true;

            if (takeFocus()) {
                if (event.type() == Event::LeftMousePressed) {
                    dragStart     = event.mousePosition();
                    isLeftPressed = true;
                    return processAction(event);
                }
                else {
                    isRightPressed = true;
                    return true;
                }
            }
            return true; // click blocked
        }
        return false;

    case Event::LeftMouseReleased:
    case Event::RightMouseReleased:
        if (!active()) {
            isLeftPressed = isRightPressed = false;
            return eventOnMe;
        }
        processAction(event);
        if (event.type() == Event::LeftMouseReleased) {
            if (isLeftPressed) {
                isLeftPressed = false;
                if (eventOnMe) {
                    processAction(Event(Event::LeftClicked, event.mousePosition()));
                    return true;
                }
            }
        }
        else if (event.type() == Event::RightMouseReleased) {
            if (isRightPressed) {
                isRightPressed = false;
                if (eventOnMe) {
                    processAction(Event(Event::RightClicked, event.mousePosition()));
                    return true;
                }
            }
        }
        return false;

    case Event::MouseMoved:
        if (!active()) { return eventOnMe; }
        if (eventOnMe) { fireSignal(event); }
        if (isLeftPressed) {
            isMouseOver  = eventOnMe;
            const bool r = processAction(Event(Event::Dragged, dragStart, event.mousePosition()));
            dragStart    = event.mousePosition();
            return r;
        }
        else if (eventOnMe) {
            if (!isMouseOver) {
                isMouseOver = true;
                processAction(Event(Event::MouseEntered, event.mousePosition()));
            }
            return true;
        }
        else if (isMouseOver) {
            isMouseOver = false;
            processAction(Event(Event::MouseLeft, event.mousePosition()));
        }
        return false;

    case Event::MouseOutsideWindow:
        isMouseOver    = false;
        isLeftPressed  = false;
        isRightPressed = false;
        return false;

    default:
        return false;
    }
}

bool Element::processAction(const Event& action) {
    if (hasFocus()) {
        fireSignal(action);
        return true;
    }
    return false;
}

bool Element::handleScroll(const Event&) { return false; }

bool Element::propagateEvent(const Event&) { return false; }

void Element::fireSignal(const Event& action) { signals[action.type()](action, this); }

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
        // TODO - stop this bubbling up at container level if requisition unchanged
        if (p) p->makeDirty();
    }
}

void Element::markClean() { _dirty = false; }

void Element::setVisible(bool v, bool md) {
    if (v != _visible && md) makeDirty();
    _visible = v;
}

bool Element::packable() const {
    if (!visible() || skipPack) return false;
    return shouldPack();
}

void Element::skipPacking(bool s) { skipPack = s; }

bool Element::shouldPack() const { return true; }

bool Element::visible() const { return _visible; }

void Element::setActive(bool a) {
    _active = a;
    if (!_active) {
        isMouseOver    = false;
        isLeftPressed  = false;
        isRightPressed = false;
    }
}

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
    fireSignal(Event(Event::AcquisitionChanged));
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

const RenderSettings& Element::renderSettings() const { return settings; }

void Element::setFont(bl::resource::Resource<sf::Font>::Ref f) {
    settings.font = f;
    fireSignal(Event(Event::RenderSettingsChanged));
}

void Element::setCharacterSize(unsigned int s) {
    settings.characterSize = s;
    fireSignal(Event(Event::RenderSettingsChanged));
}

void Element::setColor(sf::Color fill, sf::Color outline) {
    settings.fillColor    = fill;
    settings.outlineColor = outline;
    fireSignal(Event(Event::RenderSettingsChanged));
}

void Element::setOutlineThickness(unsigned int t) {
    settings.outlineThickness = t;
    fireSignal(Event(Event::RenderSettingsChanged));
}

void Element::setSecondaryColor(sf::Color fill, sf::Color outline) {
    settings.secondaryFillColor    = fill;
    settings.secondaryOutlineColor = outline;
    fireSignal(Event(Event::RenderSettingsChanged));
}

void Element::setSecondaryOutlineThickness(unsigned int t) {
    settings.secondaryOutlineThickness = t;
    fireSignal(Event(Event::RenderSettingsChanged));
}

void Element::setStyle(sf::Uint32 style) {
    settings.style = style;
    fireSignal(Event(Event::RenderSettingsChanged));
}

void Element::setHorizontalAlignment(RenderSettings::Alignment align) {
    settings.horizontalAlignment = align;
    fireSignal(Event(Event::RenderSettingsChanged));
}

void Element::setVerticalAlignment(RenderSettings::Alignment align) {
    settings.verticalAlignment = align;
    fireSignal(Event(Event::RenderSettingsChanged));
}

Element::Ptr Element::me() { return shared_from_this(); }

void Element::bringToTop(const Element*) {}

void Element::removeChild(const Element*) {}

void Element::update(float) {}

} // namespace gui
} // namespace bl