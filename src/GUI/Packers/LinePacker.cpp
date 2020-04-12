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

    sf::Vector2i pos(rect.left, rect.top);
    if (mode == Compact) {
        for (Element::Ptr e : elems) {
            sf::Vector2i size(e->getRequisition().x, maxSize.y);
            if (dir == Vertical) size = {maxSize.x, e->getRequisition().y};
            if (pos.x + size.x > rect.left + rect.width ||
                pos.y + size.y > rect.top + rect.height)
                std::cout << "Warning: LinePacker: Element minimum sizes exceed assigned "
                             "acquisition\n";
            setAcquisition(e, {pos, size});
            if (dir == Horizontal)
                pos.x += e->getRequisition().x;
            else
                pos.y += e->getRequisition().y;
        }
    }
    else if (mode == Uniform || mode == Fill) {
        sf::Vector2i size = maxSize;
        if (mode == Fill) {
            if (dir == Horizontal)
                size.x = rect.width / elems.size();
            else
                size.y = rect.height / elems.size();
        }
        if (dir == Horizontal && size.x * elems.size() > rect.width ||
            dir == Vertical && size.y * elems.size() > rect.height)
            std::cout << "Warning: LinePacker: Acquisition for elements is not enough\n";

        for (Element::Ptr e : elems) {
            setAcquisition(e, {pos, size});
            if (dir == Horizontal)
                pos.x += size.x;
            else
                pos.y += size.y;
        }
    }
    else
        std::cout << "Error: Packer has invalid mode: " << mode << std::endl;
}

} // namespace gui
} // namespace bl