#include <BLIB/Interfaces/GUI/Elements/Element.hpp>

#include <BLIB/Interfaces/GUI/Elements/Window.hpp>
#include <BLIB/Interfaces/GUI/GUI.hpp>
#include <BLIB/Logging.hpp>
#include <cmath>

namespace bl
{
namespace gui
{
Element::Element()
: renderer(nullptr)
, component(nullptr)
, parent(nullptr)
, showingTooltip(false)
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
, hoverTime(0.f) {}

Element::~Element() {
    if (component && renderer && rendererAlive && *rendererAlive) {
        renderer->destroyComponent(*this);
    }
}

void Element::setRequisition(const sf::Vector2f& size) {
    requisition.reset();
    if (size.x > 0.f && size.y > 0.f) {
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
            if (component) { component->flash(); }
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
        return processAction(event);

    case Event::LeftMousePressed:
    case Event::RightMousePressed:
        if (eventOnMe) {
            if (!active()) return true;

            if (takeFocus()) {
                if (event.type() == Event::LeftMousePressed) {
                    dragStart     = event.mousePosition();
                    isLeftPressed = true;
                    updateUiState();
                    return processAction(event);
                }
                else {
                    isRightPressed = true;
                    updateUiState();
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
                updateUiState();
                if (eventOnMe) {
                    processAction(Event(Event::LeftClicked, event.mousePosition()));
                    return true;
                }
            }
        }
        else if (event.type() == Event::RightMouseReleased) {
            if (isRightPressed) {
                isRightPressed = false;
                updateUiState();
                if (eventOnMe) {
                    processAction(Event(Event::RightClicked, event.mousePosition()));
                    return true;
                }
            }
        }
        return false;

    case Event::MouseMoved:
        if (!active()) { return eventOnMe; }
        if (showingTooltip && component) {
            showingTooltip = false;
            component->dismissTooltip();
        }
        if (eventOnMe) { fireSignal(event); }
        if (isLeftPressed) {
            isMouseOver  = eventOnMe;
            const bool r = processAction(Event(Event::Dragged, dragStart, event.mousePosition()));
            dragStart    = event.mousePosition();
            updateUiState();
            return r;
        }
        else if (eventOnMe) {
            if (!isMouseOver) {
                isMouseOver = true;
                updateUiState();
                processAction(Event(Event::MouseEntered, event.mousePosition()));
            }
            hoverTime = 0.f;
            return true;
        }
        else if (isMouseOver) {
            isMouseOver = false;
            updateUiState();
            processAction(Event(Event::MouseLeft, event.mousePosition()));
        }
        return false;

    case Event::MouseOutsideWindow:
        isMouseOver    = false;
        isLeftPressed  = false;
        isRightPressed = false;
        updateUiState();
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
    if (child->packable() && shouldMarkSelfDirty()) {
        BL_LOG_INFO << "Parent marked dirty";
        makeDirty();
    }
}

bool Element::shouldMarkSelfDirty() {
    constexpr float ShrinkThresh = 0.75f;
    const sf::Vector2f newReq    = getRequisition();
    const sf::FloatRect& acq     = getAcquisition();
    if (newReq.x > acq.width || newReq.y > acq.height ||
        (!fillX && newReq.x < acq.width * ShrinkThresh) ||
        (!fillY && newReq.y < acq.height * ShrinkThresh)) {
        return true;
    }
    return false;
}

void Element::markClean() { _dirty = false; }

void Element::setVisible(bool v, bool md) {
    const bool was = _visible;
    _visible       = v;
    if (v != was && md) makeDirty();
    if (component) { component->setVisible(v); }
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
    updateUiState();
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
    const sf::Vector2f globalPos(cachedArea.left, cachedArea.top);
    if (parent) { position = globalPos - parent->getPosition(); }
    else { position = globalPos; }
    fireSignal(Event(Event::AcquisitionChanged));
    if (component) { component->onAcquisition(); }
}

void Element::setPosition(const sf::Vector2f& pos) {
    const sf::Vector2f diff = pos - sf::Vector2f(cachedArea.left, cachedArea.top);
    dragStart += diff;
    cachedArea.left = pos.x;
    cachedArea.top  = pos.y;
    if (parent) { position = pos - parent->getPosition(); }
    else { position = pos; }
    fireSignal(Event(Event::Moved));
    if (component) { component->onMove(); }
}

void Element::recalculatePosition() {
    if (parent) {
        const sf::Vector2f npos = parent->getPosition() + position;
        if (npos.x != cachedArea.left || npos.y != cachedArea.top) {
            cachedArea.left = npos.x;
            cachedArea.top  = npos.y;
            fireSignal(Event(Event::Moved));
            if (component) { component->onMove(); }
        }
    }
}

const sf::FloatRect& Element::getAcquisition() const { return cachedArea; }

sf::Vector2f Element::getPosition() const { return {cachedArea.left, cachedArea.top}; }

const sf::Vector2f& Element::getLocalPosition() const { return position; }

void Element::setChildParent(Element* p) { p->parent = this; }

const RenderSettings& Element::renderSettings() const { return settings; }

void Element::setFont(bl::resource::Ref<sf::VulkanFont> f) {
    settings.font = f;
    onRenderChange();
}

void Element::setCharacterSize(unsigned int s) {
    settings.characterSize = s;
    onRenderChange();
}

void Element::setColor(sf::Color fill, sf::Color outline) {
    settings.fillColor    = fill;
    settings.outlineColor = outline;
    onRenderChange();
}

void Element::setOutlineThickness(float t) {
    settings.outlineThickness = t;
    onRenderChange();
}

void Element::setSecondaryColor(sf::Color fill, sf::Color outline) {
    settings.secondaryFillColor    = fill;
    settings.secondaryOutlineColor = outline;
    onRenderChange();
}

void Element::setSecondaryOutlineThickness(float t) {
    settings.secondaryOutlineThickness = t;
    onRenderChange();
}

void Element::setStyle(sf::Uint32 style) {
    settings.style = style;
    onRenderChange();
}

void Element::setHorizontalAlignment(RenderSettings::Alignment align) {
    settings.horizontalAlignment = align;
    onRenderChange();
}

void Element::setVerticalAlignment(RenderSettings::Alignment align) {
    settings.verticalAlignment = align;
    onRenderChange();
}

Element::Ptr Element::me() { return shared_from_this(); }

void Element::bringToTop(const Element*) {}

void Element::removeChild(const Element*) {}

void Element::update(float dt) {
    hoverTime += dt;

    if (component && mouseOver() && hoverTime > 1.f && !tooltip.empty() && !showingTooltip) {
        showingTooltip = true;
        component->showTooltip();
    }
}

void Element::setTooltip(const std::string& tt) { tooltip = tt; }

const std::string& Element::getTooltip() const { return tooltip; }

void Element::queueUpdateAction(const QueuedAction& a) {
    GUI* g = getTopParent();
    if (g) { g->queueAction(a); }
    else { BL_LOG_ERROR << "Element " << this << " is not a child of GUI"; }
}

GUI* Element::getTopParent() {
    Element* p = parent;
    while (p && p->parent) { p = p->parent; }
    if (p) return dynamic_cast<GUI*>(p);
    return nullptr;
}

const GUI* Element::getTopParent() const { return const_cast<Element*>(this)->getTopParent(); }

bool Element::receivesOutOfBoundsEvents() const { return false; }

void Element::updateUiState() {
    using S = rdr::Component::UIState;

    if (component) {
        if (!active()) { component->setUIState(S::Disabled); }
        else if (isLeftPressed || isRightPressed) { component->setUIState(S::Pressed); }
        else if (isMouseOver) { component->setUIState(S::Highlighted); }
        else { component->setUIState(S::Regular); }
    }
}

void Element::prepareRender(rdr::Renderer& r) {
    renderer      = &r;
    rendererAlive = r.getAliveFlag();
    if (!component) { component = doPrepareRender(r); }
    else { r.addComponentToOverlayIfRequired(component); }
    prepareChildrenRender(r);
    updateUiState();

    if (highlightBehvaiorOverride.has_value()) {
        component->overrideHighlightBehavior(highlightBehvaiorOverride.value());
    }
}

void Element::prepareChildrenRender(rdr::Renderer&) {}

void Element::onRenderChange() {
    fireSignal(Event(Event::RenderSettingsChanged));
    if (component) { component->onRenderSettingChange(); }
}

float Element::getDepthBias() const { return 0.f; }

void Element::overrideHighlightBehavior(rdr::Component::HighlightState behavior) {
    highlightBehvaiorOverride = behavior;
    if (component) { component->overrideHighlightBehavior(behavior); }
}

bool Element::isInParentTree(const Element* p) const {
    Element* cp = parent;
    while (cp != nullptr) {
        if (p == cp) { return true; }
        cp = cp->parent;
    }
    return false;
}

Element* Element::getParent() const { return parent; }

} // namespace gui
} // namespace bl
