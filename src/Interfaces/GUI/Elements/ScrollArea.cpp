#include <BLIB/Interfaces/GUI/Elements/ScrollArea.hpp>

#include <BLIB/Interfaces/Utilities.hpp>
#include <BLIB/Logging.hpp>

namespace bl
{
namespace gui
{
namespace
{
constexpr unsigned int BarSize = 16;
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

ScrollArea::Ptr ScrollArea::create(Packer::Ptr packer, const std::string& g, const std::string& i) {
    Ptr area(new ScrollArea(packer, g, i));
    area->addBars();
    return area;
}

ScrollArea::ScrollArea(Packer::Ptr packer, const std::string& g, const std::string& i)
: Container(g, i)
, packer(packer)
, horScrollbar(Slider::create(Slider::Horizontal, g, i + "-hscroll"))
, vertScrollbar(Slider::create(Slider::Vertical, g, i + "-vscroll")) {
    horScrollbar->getSignal(Action::ValueChanged)
        .willAlwaysCall(std::bind(&ScrollArea::scrolled, this));
    vertScrollbar->getSignal(Action::ValueChanged)
        .willAlwaysCall(std::bind(&ScrollArea::scrolled, this));

    horScrollbar->skipPacking(true);
    horScrollbar->setExpandsHeight(true);
    horScrollbar->setExpandsWidth(true);
    vertScrollbar->skipPacking(true);
    vertScrollbar->setExpandsHeight(true);
    vertScrollbar->setExpandsWidth(true);

    filter.insert(horScrollbar.get());
    filter.insert(vertScrollbar.get());

    using namespace std::placeholders;
    getSignal(Action::Scrolled).willAlwaysCall(std::bind(&ScrollArea::mouseScroll, this, _1));
}

void ScrollArea::addBars() {
    add(vertScrollbar);
    add(horScrollbar);
}

void ScrollArea::pack(Element::Ptr e) { add(e); }

void ScrollArea::pack(Element::Ptr e, bool fx, bool fy) {
    e->setExpandsWidth(fx);
    e->setExpandsHeight(fy);
    pack(e);
}

void ScrollArea::setAlwaysShowHorizontalScrollbar(bool s) { alwaysShowH = s; }

void ScrollArea::setAlwaysShowVerticalScrollbar(bool s) { alwaysShowV = s; }

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
    sf::Vector2i req = totalSize;
    if (maxSize.has_value())
        req = {std::min(totalSize.x, maxSize.value().x), std::min(totalSize.y, maxSize.value().y)};
    return req;
}

void ScrollArea::onAcquisition() {
    refreshSize();

    packer->pack({sf::Vector2i(0, 0), totalSize}, getPackableChildren());
    if (totalSize.x > (getAcquisition().width - BarSize) || alwaysShowH) {
        availableSize.y -= BarSize;
        const sf::Vector2i barSize(getAcquisition().width - BarSize, BarSize);
        const sf::Vector2i barPos(0, getAcquisition().height - barSize.y);
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
        const sf::Vector2i barPos(getAcquisition().width - barSize.x, 0);
        vertScrollbar->setVisible(true);
        vertScrollbar->setSliderSize(computeButtonSize(totalSize.y, availableSize.y));
        vertScrollbar->setSliderIncrement(computeIncrement(totalSize.y, availableSize.y));
        Packer::manuallyPackElement(vertScrollbar, {barPos, barSize});
    }
    else
        vertScrollbar->setVisible(false);
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
}

void ScrollArea::mouseScroll(const Action& scroll) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
        horScrollbar->incrementValue(-scroll.data.scroll);
    else
        vertScrollbar->incrementValue(-scroll.data.scroll);
}

sf::Vector2f ScrollArea::getElementOffset(const Element* e) const {
    if (e != horScrollbar.get() && e != vertScrollbar.get()) return offset;
    return {0, 0};
}

void ScrollArea::doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const {
    // Preserve old view and compute new
    const sf::View oldView = target.getView();
    sf::View view          = computeView(
        oldView, {sf::Vector2i(getAcquisition().left, getAcquisition().top), availableSize}, false);
    view.move(-offset);
    target.setView(view);

    // Render children
    renderChildrenRawFiltered(target, states, renderer, filter);

    // Compute and set scrollbar view
    view = computeView(oldView,
                       {getAcquisition().left,
                        getAcquisition().top,
                        getAcquisition().width,
                        getAcquisition().height + 16});
    target.setView(view);

    // Render scrollbars
    horScrollbar->render(target, states, renderer);
    vertScrollbar->render(target, states, renderer);

    // Restore old view
    target.setView(oldView);
}

} // namespace gui
} // namespace bl