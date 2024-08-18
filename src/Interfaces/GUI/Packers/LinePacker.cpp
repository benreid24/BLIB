#include <BLIB/Interfaces/GUI/Packers/LinePacker.hpp>

#include <cmath>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace gui
{
LinePacker::LinePacker(Direction dir, int spacing, SpaceMode mode, PackStart start)
: dir(dir)
, mode(mode)
, start(start)
, spacing(spacing) {}

Packer::Ptr LinePacker::create(Direction dir, float spacing, SpaceMode mode, PackStart start) {
    return Packer::Ptr(new LinePacker(dir, spacing, mode, start));
}

sf::Vector2f LinePacker::getRequisition(const std::vector<Element::Ptr>& elems) {
    float md = 0.f;
    float od = 0.f;
    for (const Element::Ptr& e : elems) {
        if (!e->packable()) continue;

        const auto req = e->getRequisition();
        if (dir == Vertical) {
            md += req.y;
            if (req.x > od) od = req.x;
        }
        else {
            md += req.x;
            if (req.y > od) od = req.y;
        }
    }

    md += std::max(static_cast<float>((elems.empty() ? 0 : elems.size() - 1)) * spacing, 0.f);
    if (dir == Vertical) return {od, md};
    return {md, od};
}

void LinePacker::pack(const sf::FloatRect& rect, const std::vector<Element::Ptr>& elems) {
    if (elems.empty()) return;

    sf::Vector2f maxSize;
    sf::Vector2f totalSize;
    int expanders = 0;
    for (const Element::Ptr& e : elems) {
        if (!e->packable()) continue;

        const auto req = e->getRequisition();
        if (req.x > maxSize.x) maxSize.x = req.x;
        if (req.y > maxSize.y) maxSize.y = req.y;
        if (e->expandsHeight() && dir == Vertical) ++expanders;
        if (e->expandsWidth() && dir == Horizontal) ++expanders;
        totalSize += req + sf::Vector2f(spacing, spacing);
    }
    const sf::Vector2f freeSpace = sf::Vector2f(rect.width, rect.height) - totalSize;
    const float extraSpace = expanders > 0.f ? ((dir == Horizontal) ? (freeSpace.x / expanders) :
                                                                      (freeSpace.y / expanders)) :
                                               0.f;

    sf::Vector2f pos(rect.left, rect.top);
    if (mode == Compact) {
        auto compSize = [this, &maxSize, &extraSpace, &rect](Element::Ptr e) -> sf::Vector2f {
            const auto req = e->getRequisition();
            sf::Vector2f size(req.x, maxSize.y);
            if (dir == Vertical) size = {maxSize.x, req.y};

            if (dir == Vertical) {
                if (e->expandsHeight()) size.y += extraSpace;
                if (e->expandsWidth()) size.x = rect.width;
            }
            if (dir == Horizontal) {
                if (e->expandsWidth()) size.x += extraSpace;
                if (e->expandsHeight()) size.y = rect.height;
            }
            return size;
        };

        // Compute start position
        if (start == RightAlign) {
            const sf::Vector2f size = compSize(elems.front());
            if (dir == Horizontal)
                pos.x = rect.left + rect.width - size.x;
            else
                pos.y = rect.top + rect.height - size.y;
        }

        // Pack elements
        for (unsigned int i = 0; i < elems.size(); ++i) {
            auto& e = elems[i];
            if (!e->packable()) continue;

            // Compute size
            const sf::Vector2f size = compSize(e);

            // Pack
            packElementIntoSpace(e, {pos, size});

            // Update position
            if (dir == Horizontal) {
                if (start == LeftAlign)
                    pos.x += size.x + spacing;
                else {
                    const sf::Vector2f as = i < elems.size() - 1 ? compSize(elems[i + 1]) : size;
                    pos.x -= as.x + spacing;
                }
            }
            else {
                if (start == LeftAlign)
                    pos.y += size.y + spacing;
                else {
                    const sf::Vector2f as = i < elems.size() - 1 ? compSize(elems[i + 1]) : size;
                    pos.y -= as.y + spacing;
                }
            }
        }
    }
    else if (mode == Uniform) {
        // Compute size
        sf::Vector2f size(rect.width, rect.height);
        if (dir == Horizontal)
            size.x = rect.width / elems.size() - spacing;
        else
            size.y = rect.height / elems.size() - spacing;

        // Compute oversize amount
        sf::Vector2f oversize(0.f, 0.f);
        const std::size_t en = elems.size() > 1 ? elems.size() - 1 : 1;
        const float enf      = static_cast<float>(en);
        for (const Element::Ptr& e : elems) {
            const sf::Vector2f req = e->getRequisition();
            if (dir == Horizontal && req.x > size.x) { size.x -= (req.x - size.x) / enf; }
            else if (dir == Vertical && req.y > size.y) { size.y -= (req.y - size.y) / enf; }
        }

        const auto computeElementSize = [&size](const Element::Ptr& e) -> sf::Vector2f {
            const sf::Vector2f req = e->getRequisition();
            return sf::Vector2f(std::max(size.x, req.x), std::max(size.y, req.y));
        };

        // Compute start position
        if (start == RightAlign && !elems.empty()) {
            if (dir == Horizontal)
                pos.x = rect.left + rect.width - computeElementSize(elems.front()).x;
            else
                pos.y = rect.top + rect.height - computeElementSize(elems.front()).y;
        }

        // Pack elements
        float space = 0.f;
        for (const Element::Ptr& e : elems) {
            if (!e->packable()) continue;

            const sf::Vector2f eSize = computeElementSize(e);
            packElementIntoSpace(e, {pos, eSize});

            // Update position
            if (dir == Horizontal) {
                if (start == LeftAlign)
                    pos.x += eSize.x + space;
                else
                    pos.x -= eSize.x + space;
            }
            else {
                if (start == LeftAlign)
                    pos.y += eSize.y + space;
                else
                    pos.y -= eSize.y + space;
            }
        }
    }
}

} // namespace gui
} // namespace bl
