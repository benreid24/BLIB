#include <BLIB/Interfaces/GUI/Elements/ScrollArea.hpp>

#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>
#include <BLIB/Interfaces/Utilities.hpp>

namespace bl
{
namespace gui
{
namespace
{
constexpr float BarSize        = 16;
constexpr float PixelsPerClick = 10;

float computeButtonSize(float totalSize, float availableSize) {
    float s = availableSize / totalSize;
    if (s > 1.f)
        s = 1.f;
    else if (s < 0.14f)
        s = 0.14f;
    return s;
}

float computeIncrement(float totalSize, float availableSize) {
    const float scrollSize = totalSize - availableSize;
    return PixelsPerClick / scrollSize;
}

} // namespace

ScrollArea::Ptr ScrollArea::create(const Packer::Ptr& packer) {
    return Ptr(new ScrollArea(packer));
}

ScrollArea::ScrollArea(const Packer::Ptr& packer)
: CompositeElement<3>()
, horScrollbar(Slider::create(Slider::Horizontal))
, vertScrollbar(Slider::create(Slider::Vertical))
, contentWrapper(Box::create(packer))
, content(Box::create(packer))
, alwaysShowH(false)
, alwaysShowV(false)
, neverShowH(false)
, neverShowV(false)
, includeBars(false) {
    horScrollbar->getSignal(Event::ValueChanged)
        .willAlwaysCall(std::bind(&ScrollArea::scrolled, this));
    vertScrollbar->getSignal(Event::ValueChanged)
        .willAlwaysCall(std::bind(&ScrollArea::scrolled, this));

    horScrollbar->skipPacking(true);
    horScrollbar->setExpandsHeight(true);
    horScrollbar->setExpandsWidth(true);
    vertScrollbar->skipPacking(true);
    vertScrollbar->setExpandsHeight(true);
    vertScrollbar->setExpandsWidth(true);

    content->setConstrainView(false);
    content->setOutlineThickness(0.f);

    contentWrapper->setConstrainView(true);
    contentWrapper->setOutlineThickness(0.f);
    contentWrapper->pack(content);

    Element* childs[3] = {contentWrapper.get(), horScrollbar.get(), vertScrollbar.get()};
    registerChildren(childs);
}

void ScrollArea::pack(const Element::Ptr& e) { content->pack(e); }

void ScrollArea::pack(const Element::Ptr& e, bool fx, bool fy) { content->pack(e, fx, fy); }

void ScrollArea::setAlwaysShowHorizontalScrollbar(bool s) { alwaysShowH = s; }

void ScrollArea::setAlwaysShowVerticalScrollbar(bool s) { alwaysShowV = s; }

void ScrollArea::setNeverShowHorizontalScrollbar(bool s) { neverShowH = s; }

void ScrollArea::setNeverShowVerticalScrollbar(bool s) { neverShowV = s; }

void ScrollArea::includeScrollbarsInRequisition(bool i) { includeBars = i; }

void ScrollArea::setScroll(const sf::Vector2f& scroll) {
    if (dirty()) refreshSize();
    const sf::Vector2f freeSpace = totalSize - availableSize;
    offset.x                     = freeSpace.x * scroll.x;
    offset.y                     = freeSpace.y * scroll.y;

    if (offset.x > freeSpace.x) offset.x = freeSpace.x;
    if (offset.x < 0.f) offset.x = 0.f;
    if (offset.y > freeSpace.y) offset.y = freeSpace.y;
    if (offset.y < 0.f) offset.y = 0.f;

    updateContentPos();
}

void ScrollArea::setMaxSize(const sf::Vector2f& s) {
    if (s.x <= 0.f || s.y <= 0.f)
        maxSize.reset();
    else
        maxSize = s;

    makeDirty();
}

void ScrollArea::refreshSize() const {
    totalSize     = content->getRequisition();
    availableSize = {getAcquisition().width, getAcquisition().height};
}

sf::Vector2f ScrollArea::minimumRequisition() const {
    refreshSize();
    sf::Vector2f req = totalSize;
    if (maxSize.has_value()) {
        req = {std::min(totalSize.x, maxSize.value().x), std::min(totalSize.y, maxSize.value().y)};
    }
    if (includeBars) {
        req.x += BarSize;
        req.y += BarSize;
    }
    req.x += 4.f; // it works lol
    return req;
}

void ScrollArea::onAcquisition() {
    refreshSize();

    bool showH = alwaysShowH && !neverShowH;
    if (!neverShowH) { showH = showH || availableSize.x < totalSize.x; }
    bool showV = alwaysShowV && !neverShowV;
    if (!neverShowV) { showV = showV || availableSize.y < totalSize.y; }

    if (showV) {
        availableSize.x = getAcquisition().width - BarSize;
        if (!neverShowH) showH = showH || availableSize.x < totalSize.x;
    }
    if (showH) {
        availableSize.y = getAcquisition().height - BarSize;
        if (!!neverShowV) showV = showV || availableSize.y < totalSize.y;
    }
    // one more time in case V is now visible
    if (showV) {
        availableSize.x = getAcquisition().width - BarSize;
        if (!neverShowH) showH = showH || availableSize.x < totalSize.x;
    }

    if (showH) {
        const sf::Vector2f barSize(getAcquisition().width - BarSize, BarSize);
        const sf::Vector2f barPos(getAcquisition().left,
                                  getAcquisition().top + getAcquisition().height - barSize.y);
        horScrollbar->setVisible(true);
        horScrollbar->setSliderSize(computeButtonSize(totalSize.x, availableSize.x));
        horScrollbar->setSliderIncrement(computeIncrement(totalSize.x, availableSize.x));
        Packer::manuallyPackElement(horScrollbar, {barPos, barSize}, true);
    }
    else
        horScrollbar->setVisible(false);

    if (showV) {
        const sf::Vector2f barSize(BarSize, getAcquisition().height - BarSize);
        const sf::Vector2f barPos(getAcquisition().left + getAcquisition().width - barSize.x,
                                  getAcquisition().top);
        vertScrollbar->setVisible(true);
        vertScrollbar->setSliderSize(computeButtonSize(totalSize.y, availableSize.y));
        vertScrollbar->setSliderIncrement(computeIncrement(totalSize.y, availableSize.y));
        Packer::manuallyPackElement(vertScrollbar, {barPos, barSize}, true);
    }
    else
        vertScrollbar->setVisible(false);

    Packer::manuallyPackElement(contentWrapper, {getPosition(), availableSize}, true);
    const sf::Vector2f contentArea(std::max(totalSize.x, availableSize.x),
                                   std::max(totalSize.y, availableSize.y));
    Packer::manuallyPackElement(content, {getPosition(), contentArea}, true);
}

void ScrollArea::scrolled() {
    offset = {0, 0};
    if (horScrollbar->visible()) {
        const float freeSpace = totalSize.x - availableSize.x;
        offset.x              = -freeSpace * horScrollbar->getValue();
    }
    if (vertScrollbar->visible()) {
        const float freeSpace = totalSize.y - availableSize.y;
        offset.y              = -freeSpace * vertScrollbar->getValue();
    }

    updateContentPos();

    const Event mockMove(Event::MouseMoved, boxMousePos);
    content->processEvent(mockMove);
}

bool ScrollArea::handleScroll(const Event& scroll) {
    if (getAcquisition().contains(scroll.mousePosition())) {
        if (content->handleScroll(scroll)) return true;

        if (totalSize.x > availableSize.x || totalSize.y > availableSize.y) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
                horScrollbar->incrementValue(-scroll.scrollDelta());
            }
            else { vertScrollbar->incrementValue(-scroll.scrollDelta()); }
        }
        return true;
    }
    return false;
}

bool ScrollArea::propagateEvent(const Event& event) {
    if (event.type() == Event::MouseMoved) { boxMousePos = event.mousePosition(); }

    if (horScrollbar->processEvent(event)) return true;
    if (vertScrollbar->processEvent(event)) return true;

    const bool in =
        getAcquisition().contains(event.mousePosition()) || content->receivesOutOfBoundsEvents();
    if (in || event.type() == Event::MouseMoved || event.type() == Event::LeftMouseReleased ||
        event.type() == Event::RightMouseReleased) {
        return content->processEvent(event);
    }
    return false;
}

rdr::Component* ScrollArea::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<ScrollArea>(*this);
}

void ScrollArea::updateContentPos() { content->setPosition(getPosition() + offset); }

void ScrollArea::clearChildren(bool immediate) { content->clearChildren(immediate); }

bool ScrollArea::receivesOutOfBoundsEvents() const { return content->receivesOutOfBoundsEvents(); }

} // namespace gui
} // namespace bl
