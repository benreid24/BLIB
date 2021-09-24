#include <BLIB/Interfaces/GUI/Elements/ScrollArea.hpp>

#include <BLIB/Interfaces/Utilities.hpp>

namespace bl
{
namespace gui
{
namespace
{
constexpr int BarSize          = 16;
constexpr float PixelsPerClick = 10;

float computeButtonSize(float totalSize, float availableSize) {
    float s = availableSize / totalSize;
    if (s > 1)
        s = 1;
    else if (s < 0.14)
        s = 0.14;
    return s;
}

float computeIncrement(float totalSize, float availableSize) {
    const float scrollSize = totalSize - availableSize;
    return PixelsPerClick / scrollSize;
}

} // namespace

ScrollArea::Ptr ScrollArea::create(Packer::Ptr packer) { return Ptr(new ScrollArea(packer)); }

ScrollArea::ScrollArea(Packer::Ptr packer)
: Container()
, packer(packer)
, horScrollbar(Slider::create(Slider::Horizontal))
, vertScrollbar(Slider::create(Slider::Vertical)) {
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
}

void ScrollArea::pack(Element::Ptr e) { add(e); }

void ScrollArea::pack(Element::Ptr e, bool fx, bool fy) {
    e->setExpandsWidth(fx);
    e->setExpandsHeight(fy);
    pack(e);
}

void ScrollArea::setAlwaysShowHorizontalScrollbar(bool s) { alwaysShowH = s; }

void ScrollArea::setAlwaysShowVerticalScrollbar(bool s) { alwaysShowV = s; }

void ScrollArea::setScroll(const sf::Vector2f& scroll) {
    if (dirty()) refreshSize();
    const sf::Vector2i freeSpace = totalSize - availableSize;
    offset.x                     = static_cast<float>(freeSpace.x) * scroll.x;
    offset.y                     = static_cast<float>(freeSpace.y) * scroll.y;

    if (offset.x > freeSpace.x) offset.x = freeSpace.x;
    if (offset.x < 0.f) offset.x = 0.f;
    if (offset.y > freeSpace.y) offset.y = freeSpace.y;
    if (offset.y < 0.f) offset.y = 0.f;
}

void ScrollArea::setMaxSize(const sf::Vector2i& s) {
    if (s.x <= 0 || s.y <= 0)
        maxSize.reset();
    else
        maxSize = s;

    makeDirty();
}

void ScrollArea::refreshSize() const {
    totalSize     = packer->getRequisition(getPackableChildren());
    availableSize = {getAcquisition().width, getAcquisition().height};
}

sf::Vector2i ScrollArea::minimumRequisition() const {
    refreshSize();
    sf::Vector2i req = totalSize + sf::Vector2i(BarSize, BarSize);
    if (maxSize.has_value())
        req = {std::min(totalSize.x, maxSize.value().x), std::min(totalSize.y, maxSize.value().y)};
    return req;
}

void ScrollArea::onAcquisition() {
    refreshSize();

    if (totalSize.x > (getAcquisition().width - BarSize) || alwaysShowH) {
        availableSize.y -= BarSize;
        const sf::Vector2i barSize(getAcquisition().width - BarSize, BarSize);
        const sf::Vector2i barPos(getAcquisition().left,
                                  getAcquisition().top + getAcquisition().height - barSize.y);
        horScrollbar->setVisible(true);
        horScrollbar->setSliderSize(computeButtonSize(totalSize.x, availableSize.x));
        horScrollbar->setSliderIncrement(computeIncrement(totalSize.x, availableSize.x));
        Packer::manuallyPackElement(horScrollbar, {barPos, barSize});
    }
    else
        horScrollbar->setVisible(false);

    if (totalSize.y > (getAcquisition().height - BarSize) || alwaysShowV) {
        availableSize.x -= BarSize;
        const sf::Vector2i barSize(BarSize, getAcquisition().height - BarSize);
        const sf::Vector2i barPos(getAcquisition().left + getAcquisition().width - barSize.x,
                                  getAcquisition().top);
        vertScrollbar->setVisible(true);
        vertScrollbar->setSliderSize(computeButtonSize(totalSize.y, availableSize.y));
        vertScrollbar->setSliderIncrement(computeIncrement(totalSize.y, availableSize.y));
        Packer::manuallyPackElement(vertScrollbar, {barPos, barSize});
    }
    else
        vertScrollbar->setVisible(false);

    packer->pack(
        {0, 0, std::max(totalSize.x, availableSize.x), std::max(totalSize.y, availableSize.y)},
        getPackableChildren());
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

    const Event mockMove(Event::MouseMoved, boxMousePos + offset);
    for (Element::Ptr e : getPackableChildren()) { e->processEvent(mockMove); }
}

bool ScrollArea::handleScroll(const Event& scroll) {
    if (getAcquisition().contains(sf::Vector2i(scroll.mousePosition()))) {
        if (Container::handleScroll(scroll)) return true;

        if (totalSize.x > availableSize.x || totalSize.y > availableSize.y) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
                horScrollbar->incrementValue(-scroll.scrollDelta());
            }
            else {
                vertScrollbar->incrementValue(-scroll.scrollDelta());
            }
        }
        return true;
    }
    return false;
}

sf::Vector2f ScrollArea::getElementOffset(const Element* e) const {
    if (e != horScrollbar.get() && e != vertScrollbar.get()) return offset;
    return {0, 0};
}

bool ScrollArea::propagateEvent(const Event& event) {
    if (event.type() == sf::Event::MouseMoved) { boxMousePos = event.mousePosition(); }

    if (horScrollbar->processEvent(event)) return true;
    if (vertScrollbar->processEvent(event)) return true;

    const bool in = getAcquisition().contains(sf::Vector2i(event.mousePosition()));
    if (in || event.type() == sf::Event::MouseMoved ||
        event.type() == sf::Event::MouseButtonReleased) {
        const sf::Vector2f pos(getAcquisition().left, getAcquisition().top);
        return Container::propagateEvent({event, event.mousePosition() - pos + offset});
    }
    return false;
}

void ScrollArea::doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const {
    // Preserve old view and compute new
    const sf::View oldView = target.getView();
    sf::View view          = interface::ViewUtil::computeSubView(
        sf::FloatRect{sf::Vector2f(getAcquisition().left, getAcquisition().top),
                      sf::Vector2f(availableSize)},
        oldView);
    view.setCenter(view.getSize() * 0.5f);
    view.move(offset);
    target.setView(view);

    // Render children
    renderChildren(target, states, renderer, false);

    // Restore old view
    target.setView(oldView);

    // Render scrollbars
    horScrollbar->render(target, states, renderer);
    vertScrollbar->render(target, states, renderer);
}

} // namespace gui
} // namespace bl
