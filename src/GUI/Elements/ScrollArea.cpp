#include <BLIB/GUI/Elements/ScrollArea.hpp>

namespace bl
{
namespace gui
{
namespace
{
constexpr unsigned int BarSize = 16;
}

ScrollArea::Ptr ScrollArea::create(Packer::Ptr packer, const std::string& g,
                                   const std::string& i) {
    Ptr area(new ScrollArea(packer, g, i));
    area->addBars();
    return area;
}

ScrollArea::ScrollArea(Packer::Ptr packer, const std::string& g, const std::string& i)
: Container(g, i)
, packer(packer)
, horScrollbar(Slider::create(Slider::Horizontal, g, i + "-hscroll"))
, vertScrollbar(Slider::create(Slider::Vertical, g, i + "vscroll")) {
    using namespace std::placeholders;
    /*horScrollbar->getSignal(Action::ValueChanged)
        .willAlwaysCall(std::bind(&ScrollArea::scrolled, this, _1, _2));
    vertScrollbar->getSignal(Action::ValueChanged)
        .willAlwaysCall(std::bind(&ScrollArea::scrolled, this, _1, _2));*/

    horScrollbar->skipPacking(true);
    vertScrollbar->skipPacking(true);

    filter.insert(horScrollbar.get());
    filter.insert(vertScrollbar.get());
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
    totalSize = packer->getRequisition(getPackableChildren());
}

sf::Vector2i ScrollArea::minimumRequisition() const {
    refreshSize();
    const sf::Vector2i mreq = maxSize.value_or(sf::Vector2i(10000000, 10000000));
    return {std::min(totalSize.x, mreq.x), std::min(totalSize.y, mreq.y)};
}

void ScrollArea::onAcquisition() {
    refreshSize();

    if (totalSize.x > getAcquisition().width || alwaysShowH) {
        const sf::Vector2i barSize(BarSize, getAcquisition().height - BarSize);
        const sf::Vector2i barPos(getAcquisition().width - barSize.x, 0);
        horScrollbar->setRequisition(barSize);
        horScrollbar->setVisible(true);
        Packer::manuallyPackElement(horScrollbar, {barPos, barSize});
    }
    else
        horScrollbar->setVisible(false);

    if (totalSize.y > getAcquisition().height || alwaysShowV) {
        const sf::Vector2i barSize(getAcquisition().width - BarSize, BarSize);
        const sf::Vector2i barPos(0, getAcquisition().height - barSize.y);
        horScrollbar->setRequisition(barSize);
        vertScrollbar->setVisible(true);
        Packer::manuallyPackElement(vertScrollbar, {barPos, barSize});
    }
    else
        vertScrollbar->setVisible(false);

    packer->pack({sf::Vector2i(0, 0), totalSize}, getPackableChildren());
}

void ScrollArea::doRender(sf::RenderTarget& target, sf::RenderStates states,
                          Renderer::Ptr renderer) const {
    // Preserve old view and compute new
    const sf::View oldView = target.getView();
    sf::View view          = computeView(target, states.transform);
    target.setView(view);

    // Calculate scroll offset
    sf::Vector2f offset(0, 0);
    if (horScrollbar->visible()) {
        const float freeSpace = totalSize.x - getAcquisition().width;
        offset.x              = -freeSpace * horScrollbar->getValue();
    }
    if (vertScrollbar->visible()) {
        const float freeSpace = totalSize.y - getAcquisition().height;
        offset.x              = -freeSpace * vertScrollbar->getValue();
    }

    // Translate transform
    states.transform.translate(getAcquisition().left, getAcquisition().height);
    states.transform.translate(offset);

    // Render children
    renderChildrenRawFiltered(target, states, renderer, filter);

    // Undo scroll offset
    states.transform.translate(offset * -1.f);

    // Render scrollbars
    horScrollbar->render(target, states, renderer);
    vertScrollbar->render(target, states, renderer);

    // Restore old view
    target.setView(oldView);
}

} // namespace gui
} // namespace bl