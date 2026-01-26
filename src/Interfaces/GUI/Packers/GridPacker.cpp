#include <BLIB/Interfaces/GUI/Packers/GridPacker.hpp>

namespace bl
{
namespace gui
{
GridPacker::Ptr GridPacker::createDynamicGrid(Direction dir, unsigned int size, float padding) {
    return Ptr(new GridPacker(Dynamic, dir, padding, size));
}

GridPacker::Ptr GridPacker::createFixedGrid(Direction dir, unsigned int wrap, float padding) {
    return Ptr(new GridPacker(FixedAmount, dir, padding, wrap));
}

GridPacker::GridPacker(WrapRule r, Direction d, float pad, unsigned int s)
: rule(r)
, dir(d)
, padding(pad)
, size(s) {}

sf::Vector2f GridPacker::getRequisition(const std::vector<Element::Ptr>& elements) {
    sf::Vector2f req(0, 0);
    auto measure = [&req](Element::Ptr, const sf::FloatRect& area) {
        if (req.x < area.position.x + area.size.x) req.x = area.position.x + area.size.x;
        if (req.y < area.position.y + area.size.y) req.y = area.position.y + area.size.y;
    };
    doPack({}, elements, measure);
    return req;
}

void GridPacker::pack(const sf::FloatRect& area, const std::vector<Element::Ptr>& elements) {
    doPack(area, elements, &Packer::packElementIntoSpace);
}

void GridPacker::doPack(const sf::FloatRect& area, const std::vector<Element::Ptr>& elements,
                        const std::function<void(Element::Ptr, const sf::FloatRect&)>& packCb) {
    sf::Vector2f pos(area.position.x, area.position.y);
    unsigned int pcount = 0;
    sf::Vector2f mreq;

    for (unsigned int i = 0; i < elements.size(); ++i) {
        if (!elements[i]->packable()) continue;

        // Pack
        const sf::Vector2f req = elements[i]->getRequisition();
        packCb(elements[i], {pos, req});
        ++pcount;

        // Track size for when we make next row/column
        mreq.x = std::max(mreq.x, req.x);
        mreq.y = std::max(mreq.y, req.y);

        // Do wrap based on rule and dir
        if (i < elements.size() - 1) {
            if (dir == Rows) {
                if (rule == FixedAmount) {
                    if (pcount == size) { // wrap
                        pos.x = area.position.x;
                        pos.y += mreq.y + padding;
                        mreq   = {0, 0};
                        pcount = 0;
                    }
                    else { // continue in row/column
                        pos.x += req.x + padding;
                    }
                }
                else {
                    const sf::Vector2f nreq = elements[i + 1]->getRequisition();
                    if (pos.x + padding + nreq.x > size) { // wrap
                        pos.x = area.position.x;
                        pos.y += mreq.y + padding;
                        mreq = {0, 0};
                    }
                    else { // continue in row/column
                        pos.x += req.x + padding;
                    }
                }
            }
            else {
                if (rule == FixedAmount) {
                    if (pcount == size) { // wrap
                        pos.y = area.position.y;
                        pos.x += mreq.x + padding;
                        mreq   = {0, 0};
                        pcount = 0;
                    }
                    else { // continue in row/column
                        pos.y += req.y + padding;
                    }
                }
                else {
                    const sf::Vector2f nreq = elements[i + 1]->getRequisition();
                    if (pos.y + padding + nreq.y > size) { // wrap
                        pos.y = area.position.y;
                        pos.x += mreq.x + padding;
                        mreq = {0, 0};
                    }
                    else { // continue in row/column
                        pos.y += req.y + padding;
                    }
                }
            }
        }
    }
}

} // namespace gui
} // namespace bl
