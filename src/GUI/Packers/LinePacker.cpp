#include <BLIB/GUI/Packers/LinePacker.hpp>

#include <cmath>

namespace bl
{
namespace gui
{
LinePacker::LinePacker(Direction dir, int spacing, SpaceMode mode, PackStart start)
: dir(dir)
, mode(mode)
, start(start)
, spacing(spacing) {}

Packer::Ptr LinePacker::create(Direction dir, int spacing, SpaceMode mode, PackStart start) {
    return Packer::Ptr(new LinePacker(dir, spacing, mode, start));
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

    md += elems.size() * spacing;
    if (dir == Vertical) return {od, md};
    return {md, od};
}

void LinePacker::pack(const sf::IntRect& rect, const std::vector<Element::Ptr>& elems) {
    if (elems.empty()) return;

    sf::Vector2i maxSize;
    sf::Vector2i totalSize;
    int expanders = 0;
    for (Element::Ptr e : elems) {
        if (!e->packable()) continue;

        if (e->getRequisition().x > maxSize.x) maxSize.x = e->getRequisition().x;
        if (e->getRequisition().y > maxSize.y) maxSize.y = e->getRequisition().y;
        if (e->expandsHeight() && dir == Vertical) ++expanders;
        if (e->expandsWidth() && dir == Horizontal) ++expanders;
        totalSize += e->getRequisition() + sf::Vector2i(spacing, spacing);
    }
    const sf::Vector2i freeSpace = sf::Vector2i(rect.width, rect.height) - totalSize;
    const int extraSpace = expanders > 0 ? ((dir == Horizontal) ? (freeSpace.x / expanders) :
                                                                  (freeSpace.y / expanders)) :
                                           0;

    sf::Vector2i pos(rect.left, rect.top);
    if (mode == Compact) {
        auto compSize = [this, &maxSize, &extraSpace](Element::Ptr e) -> sf::Vector2i {
            sf::Vector2i size(e->getRequisition().x, maxSize.y);
            if (dir == Vertical) size = {maxSize.x, e->getRequisition().y};
            if (dir == Vertical && e->expandsHeight()) size.y += extraSpace;
            if (dir == Horizontal && e->expandsWidth()) size.x += extraSpace;
            return size;
        };

        // Compute start position
        if (start == RightAlign) {
            const sf::Vector2i size = compSize(elems.back());
            if (dir == Horizontal)
                pos.x = rect.left + rect.width - size.x;
            else
                pos.y = rect.top + rect.height - size.y;
        }

        // Pack elements
        for (Element::Ptr e : elems) {
            if (!e->packable()) continue;

            // Compute size
            const sf::Vector2i size = compSize(e);

            // Pack
            packElementIntoSpace(e, {pos, size});

            // Update position
            if (dir == Horizontal) {
                if (start == LeftAlign)
                    pos.x += size.x + spacing;
                else
                    pos.x -= size.x + spacing;
            }
            else {
                if (start == LeftAlign)
                    pos.y += size.y + spacing;
                else
                    pos.y -= size.y + spacing;
            }
        }
    }
    else if (mode == Uniform) {
        // Compute size
        sf::Vector2i size(rect.width, rect.height);
        if (dir == Horizontal)
            size.x = rect.width / elems.size() - spacing;
        else
            size.y = rect.height / elems.size() - spacing;

        // Compute start position
        if (start == RightAlign) {
            if (dir == Horizontal)
                pos.x = rect.left + rect.width - size.x;
            else
                pos.y = rect.top + rect.height - size.y;
        }

        // Pack elements
        for (Element::Ptr e : elems) {
            if (!e->packable()) continue;

            packElementIntoSpace(e, {pos, size});

            // Update position
            if (dir == Horizontal) {
                if (start == LeftAlign)
                    pos.x += size.x + spacing;
                else
                    pos.x -= size.x + spacing;
            }
            else {
                if (start == LeftAlign)
                    pos.y += size.y + spacing;
                else
                    pos.y -= size.y + spacing;
            }
        }
    }
}

} // namespace gui
} // namespace bl