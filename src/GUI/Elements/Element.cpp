#include <BLIB/GUI/Elements/Element.hpp>

namespace bl
{
namespace gui
{
Element::Element(const std::string& i, const std::string& g)
: _id(i)
, _group(g)
, _dirty(true)
, _active(true)
, _visible(true)
, isFocused(false)
, focusForced(false)
, isMouseOver(false)
, isLeftPressed(false)
, isRightPressed(false) {}

const std::string& Element::id() const { return _id; }

const std::string& Element::group() const { return _group; }

void Element::setRequisition(const sf::Vector2i& size) {
    requisition.reset();
    if (size.x > 0 && size.y > 0) {
        const sf::Vector2i min = minimumRequisition();
        if (size.x >= min.x && size.y >= min.y) requisition = size;
    }
    makeDirty();
}

sf::Vector2i Element::getRequisition() const {
    return requisition.value_or(minimumRequisition());
}

const sf::IntRect& Element::getAcquisition() const { return acquisition; }

Signal& Element::getSignal(Action::Type trigger) { return signals[trigger]; }

bool Element::hasFocus() const { return isFocused; }

bool Element::takeFocus() {
    Element::Ptr p = parent.lock();
    if (p) {
        if (p->releaseFocus()) {
            isFocused = true;
            moveToTop();
            return true;
        }
        return false;
    }
    isFocused = true;
    moveToTop();
    return true;
}

bool Element::setForceFocus(bool force) {
    focusForced = force ? takeFocus() : false;
    return hasFocus() || !force;
}

bool Element::releaseFocus() {
    if (focusForced) {
        if (hasFocus()) return false;
    }
    isFocused = false;
    return true;
}

void Element::moveToTop() const {
    Element::Ptr p = parent.lock();
    if (p) p->bringToTop(this);
}

bool Element::mouseOver() const { return isMouseOver; }

bool Element::leftPressed() const { return isLeftPressed; }

bool Element::rightPressed() const { return isRightPressed; }

bool Element::handleEvent(const sf::Vector2f& mpos, const sf::Event& event) {
    if (handleRawEvent(mpos, event)) return true;

    switch (event.type) {
    case sf::Event::TextEntered:
    case sf::Event::KeyPressed:
    case sf::Event::KeyReleased:
    case sf::Event::MouseWheelScrolled:
        return processAction(Action::fromSFML(mpos, event));

    case sf::Event::MouseButtonPressed:
        if (acquisition.contains(sf::Vector2i(event.mouseButton.x, event.mouseButton.y))) {
            if (!active()) return true;

            if (takeFocus()) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    dragStart     = mpos;
                    isLeftPressed = true;
                    return processAction(Action(Action::Pressed, mpos));
                }
                else if (event.mouseButton.button == sf::Mouse::Right) {
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
            return acquisition.contains(
                sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        }
        if (event.mouseButton.button == sf::Mouse::Left) {
            if (isLeftPressed) {
                isLeftPressed = false;
                processAction(Action(Action::Released, mpos));
                if (acquisition.contains(
                        sf::Vector2i(event.mouseButton.x, event.mouseButton.y))) {
                    processAction(Action(Action::Clicked, mpos));
                    return true;
                }
            }
        }
        else if (event.mouseButton.button == sf::Mouse::Right) {
            if (isRightPressed) {
                isRightPressed = false;
                if (acquisition.contains(
                        sf::Vector2i(event.mouseButton.x, event.mouseButton.y))) {
                    processAction(Action(Action::RightClicked, mpos));
                    return true;
                }
            }
        }
        return false;

    case sf::Event::MouseMoved:
        if (!active()) {
            return acquisition.contains(
                sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        }
        if (isLeftPressed)
            return processAction(Action(Action::Dragged, dragStart, mpos));
        else if (acquisition.contains(sf::Vector2i(event.mouseMove.x, event.mouseMove.y))) {
            if (!isMouseOver) {
                isMouseOver = true;
                processAction(Action(Action::MouseEntered, mpos));
            }
            return true;
        }
        else if (isMouseOver) {
            isMouseOver = false;
            processAction(Action(Action::MouseLeft, mpos));
        }
        return false;

    default:
        return false;
    }
}

bool Element::handleRawEvent(const sf::Vector2f&, const sf::Event&) { return false; }

bool Element::processAction(const Action& action) {
    if (hasFocus() || (action.type == Action::Scrolled && mouseOver())) {
        handleAction(action);
        signals[action.type](action, this);
        return true;
    }
    return false;
}

void Element::remove() {
    Element::Ptr p = parent.lock();
    if (p) p->removeChild(this);
}

void Element::makeDirty() {
    _dirty         = true;
    Element::Ptr p = parent.lock();
    if (p) p->makeDirty();
}

void Element::setVisible(bool v) { _visible = v; }

bool Element::visible() const { return _visible; }

void Element::setActive(bool a) { _active = a; }

bool Element::active() const { return _active; }

void Element::assignAcquisition(const sf::IntRect& acq) {
    _dirty      = false;
    acquisition = acq;
}

void Element::setChildParent(Element::Ptr child) { child->parent = me(); }

void Element::render(sf::RenderTarget& target, Renderer::Ptr renderer) const {
    if (visible()) doRender(target, renderer);
}

void Element::doRender(sf::RenderTarget& target, Renderer::Ptr renderer) const {
    renderer->renderCustom(target, *this);
}

const RenderSettings& Element::renderSettings() const { return settings; }

void Element::setCharacterSize(unsigned int s) {
    settings.characterSize = s;
    settingsChanged();
}

void Element::setColor(sf::Color fill, sf::Color outline) {
    settings.fillColor    = fill;
    settings.outlineColor = outline;
    settingsChanged();
}

void Element::setOutlineThickness(unsigned int t) {
    settings.outlineThickness = t;
    settingsChanged();
}

void Element::setStyle(sf::Uint32 style) {
    settings.style = style;
    settingsChanged();
}

void Element::setHorizontalAlignment(RenderSettings::Alignment align) {
    settings.horizontalAlignment = align;
    settingsChanged();
}

void Element::setVerticalAlignment(RenderSettings::Alignment align) {
    settings.verticalAlignment = align;
    settingsChanged();
}

Element::Ptr Element::me() { return shared_from_this(); }

} // namespace gui
} // namespace bl