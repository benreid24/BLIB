#include <BLIB/Interfaces/GUI/Elements/Element.hpp>

#include <BLIB/Interfaces/GUI/GUI.hpp>
#include <BLIB/Logging.hpp>
#include <cmath>

namespace bl
{
namespace gui
{
Element::Element()
: parent(nullptr)
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
, isRightPressed(false)
, flashTime(-1.f)
, hoverTime(0.f) {}

void Element::setRequisition(const sf::Vector2f& size) {
    requisition.reset();
    if (size.x > 0 && size.y > 0) {
        requisition = size;
        makeDirty();
    }
}

sf::Vector2f Element::getRequisition() const {
    const sf::Vector2f mr = minimumRequisition();
    if (requisition.has_value())
        return {std::max(requisition.value().x, mr.x), std::max(requisition.value().y, mr.y)};
    return mr;
}

Signal& Element::getSignal(Event::Type trigger) { return signals[trigger]; }

bool Element::hasFocus() const { return isFocused; }

bool Element::focusIsforced() const { return focusForced; }

bool Element::takeFocus() {
    if (isFocused) return true;

    if (!clearFocus(this)) return false;

    isFocused = true;
    fireSignal(Event(Event::GainedFocus));
    moveToTop();
    return true;
}

bool Element::setForceFocus(bool force) {
    focusForced = force ? takeFocus() : false;
    return hasFocus() || !force;
}

bool Element::clearFocus(const Element* requester) {
    Element* p = parent;
    while (p) {
        if (!p->releaseFocus(requester)) return false;
        p = p->parent;
    }
    return releaseFocus(requester);
}

bool Element::releaseFocus(const Element* requester) {
    if (focusForced) {
        if (requester && (requester != this && !isChild(requester))) {
            if (flashTime < 0.f) flashTime = 0.f;
            return false;
        }
    }
    bool wasFocused = isFocused;
    isFocused       = false;
    if (wasFocused) fireSignal(Event(Event::LostFocus));
    return true;
}

bool Element::isChild(const Element* e) {
    const Element* p = e->parent;
    while (p) {
        if (p == this) return true;
        p = p->parent;
    }
    return false;
}

void Element::moveToTop() const {
    if (parent) parent->bringToTop(this);
}

bool Element::mouseOver() const { return isMouseOver; }

bool Element::leftPressed() const { return isLeftPressed; }

bool Element::rightPressed() const { return isRightPressed; }

bool Element::processEvent(const Event& event) {
    if (!visible()) return false;

    if (propagateEvent(event)) return true;

    const bool eventOnMe = cachedArea.contains(event.mousePosition());

    if (event.type() == Event::Scrolled) {
        if (eventOnMe) { return handleScroll(event); }
        return false;
    }

    switch (event.type()) {
    case Event::TextEntered:
    case Event::KeyPressed:
    case Event::KeyReleased:
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
            hoverTime = 0.f;
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
    if (parent) { parent->removeChild(this); }
}

void Element::makeDirty() {
    _dirty = true;
    if (parent) { parent->requestMakeDirty(this); }
}

void Element::requestMakeDirty(const Element* child) {
    if (child->packable()) makeDirty();
}

void Element::markClean() { _dirty = false; }

void Element::setVisible(bool v, bool md) {
    if (v != _visible && md) makeDirty();
    _visible = v;
}

bool Element::packable(bool ivs) const {
    if ((!visible() && !ivs) || skipPack) return false;
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

void Element::assignAcquisition(const sf::FloatRect& acq) {
    markClean();
    cachedArea = acq;
    if (parent) {
        position = sf::Vector2f(cachedArea.left, cachedArea.top) - parent->getPosition();
    }
    fireSignal(Event(Event::AcquisitionChanged));
}

void Element::setPosition(const sf::Vector2f& pos) {
    const sf::Vector2f diff = pos - sf::Vector2f(cachedArea.left, cachedArea.top);
    dragStart += diff;
    cachedArea.left = pos.x;
    cachedArea.top  = pos.y;
    if (parent) {
        position = sf::Vector2f(cachedArea.left, cachedArea.top) - parent->getPosition();
    }
    fireSignal(Event(Event::Moved));
}

void Element::recalculatePosition() {
    if (parent) {
        const sf::Vector2f npos = parent->getPosition() + position;
        if (npos.x != cachedArea.left || npos.y != cachedArea.top) {
            cachedArea.left = npos.x;
            cachedArea.top  = npos.y;
            fireSignal(Event(Event::Moved));
        }
    }
}

const sf::FloatRect& Element::getAcquisition() const { return cachedArea; }

sf::Vector2f Element::getPosition() const { return {cachedArea.left, cachedArea.top}; }

void Element::setChildParent(Element* p) { p->parent = this; }

void Element::render(sf::RenderTarget& target, sf::RenderStates states,
                     const Renderer& renderer) const {
    if (visible()) {
        doRender(target, states, renderer);
        if (flashTime >= 0.f) {
            sf::RectangleShape rect({getAcquisition().width, getAcquisition().height});
            rect.setFillColor(sf::Color(
                255, 255, 255, 50.f + std::abs(std::sin(flashTime * 4.f * 3.1415f) * 120.f)));
            rect.setPosition(getPosition());
            target.draw(rect, states);
        }
        if (mouseOver() && hoverTime > 1.f && !tooltip.empty()) {
            renderer.setTooltipToRender(this);
        }
    }
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

void Element::setOutlineThickness(float t) {
    settings.outlineThickness = t;
    fireSignal(Event(Event::RenderSettingsChanged));
}

void Element::setSecondaryColor(sf::Color fill, sf::Color outline) {
    settings.secondaryFillColor    = fill;
    settings.secondaryOutlineColor = outline;
    fireSignal(Event(Event::RenderSettingsChanged));
}

void Element::setSecondaryOutlineThickness(float t) {
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

void Element::update(float dt) {
    if (flashTime >= 0.f) {
        flashTime += dt;
        if (flashTime > 0.5f) { flashTime = -1.f; }
    }
    hoverTime += dt;
}

void Element::setTooltip(const std::string& tt) { tooltip = tt; }

const std::string& Element::getTooltip() const { return tooltip; }

void Element::queueUpdateAction(const QueuedAction& a) {
    GUI* g = getTopParent();
    if (g) { g->queueAction(a); }
    else {
        BL_LOG_ERROR << "Element " << this << " is not a child of GUI";
    }
}

GUI* Element::getTopParent() {
    Element* p = parent;
    while (p && p->parent) { p = p->parent; }
    if (p) return dynamic_cast<GUI*>(p);
    return nullptr;
}

const GUI* Element::getTopParent() const { return const_cast<Element*>(this)->getTopParent(); }

} // namespace gui
} // namespace bl
