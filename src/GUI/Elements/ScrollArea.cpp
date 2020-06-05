#include <BLIB/GUI/Elements/ScrollArea.hpp>

namespace bl
{
namespace gui
{
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
, vertScrollBar(Slider::create(Slider::Vertical, g, i + "vscroll")) {
    // TODO - init
}

void ScrollArea::addBars() {
    add(vertScrollBar);
    add(horScrollbar);
}

} // namespace gui
} // namespace bl