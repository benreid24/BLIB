#include <BLIB/GUI/Elements/Element.hpp>

namespace bl
{
namespace gui
{
Element::Element(const std::string& i, const std::string& g)
: _id(i)
, _group(g)
, _dirty(true)
, isFocused(false)
, focusForced(false)
, isMouseOver(false)
, isLeftPressed(false)
, isRightPressed(false) {}

const std::string& Element::id() const { return _id; }

const std::string& Element::group() const { return _group; }

void Element::setRequisition(const sf::Vector2f& size) {
    requisition.reset();
    if (size.x > 0 && size.y > 0) {
        const sf::Vector2f min = minimumRequisition();
        if (size.x >= min.x && size.y >= min.y) requisition = size;
    }
}

sf::Vector2f Element::getRequisition() const {
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

bool Element::setForceFocus(bool force) { focusForced = force ? takeFocus() : false; }

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
        if (isLeftPressed)
            return processAction(Action(Action::Dragged, dragStart, mpos));
        else if (acquisition.contains(sf::Vector2i(event.mouseMove.x, event.mouseMove.y))) {
            if (!isMouseOver) {
                isMouseOver = true;
                return processAction(Action(Action::MouseEntered, mpos));
            }
        }
        else if (isMouseOver) {
            isMouseOver = false;
            return processAction(Action(Action::MouseLeft, mpos));
        }

    default:
        return false;
    }
}

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

void Element::assignAcquisition(const sf::IntRect& acq) {
    _dirty      = false;
    acquisition = acq;
}

void Element::setParent(Element::Ptr p) { parent = p; }

void Element::render(sf::RenderTarget& target, Renderer::Ptr renderer) const {
    renderer->renderCustom(target, *this);
}

} // namespace gui
} // namespace bl