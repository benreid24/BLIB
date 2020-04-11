#include <BLIB/GUI/Packers/LinePacker.hpp>

#include <iostream>

namespace bl
{
namespace gui
{
LinePacker::LinePacker(Direction dir, SpaceMode mode)
: dir(dir)
, mode(mode) {}

Packer::Ptr LinePacker::create(Direction dir, SpaceMode mode) {
    return Packer::Ptr(new LinePacker(dir, mode));
}

sf::Vector2i LinePacker::getRequisition(const std::vector<Element::Ptr>& elems) {
    int md = 0;
    int od = 0;
    for (Element::Ptr e : elems) {
        if (dir == Vertical) {
            md += e->getRequisition().y;
            if (e->getRequisition().x > od) od = e->getRequisition().x;
        }
        else {
            md += e->getRequisition().x;
            if (e->getRequisition().y > od) od = e->getRequisition().y;
        }
    }

    if (dir == Vertical) return {od, md};
    return {md, od};
}

void LinePacker::pack(const sf::IntRect& rect, const std::vector<Element::Ptr>& elems) {
    sf::Vector2i maxSize;
    for (Element::Ptr e : elems) {
        if (e->getRequisition().x > maxSize.x) maxSize.x = e->getRequisition().x;
        if (e->getRequisition().y > maxSize.y) maxSize.y = e->getRequisition().y;
    }

    if (mode == Compact) {
        sf::Vector2i pos(rect.left, rect.top);
        for (Element::Ptr e : elems) {
            sf::Vector2i size(e->getRequisition().x, maxSize.y);
            if (dir == Vertical) size = {maxSize.x, e->getRequisition().y};
            setAcquisition(e, {pos, size});
            if (dir == Horizontal)
                pos.x += e->getRequisition().x;
            else
                pos.y += e->getRequisition().y;
        }
    }
    // TODO - other modes
}

} // namespace gui
} // namespace bl