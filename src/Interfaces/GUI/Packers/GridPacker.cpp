#include <BLIB/Interfaces/GUI/Packers/GridPacker.hpp>

namespace bl
{
namespace gui
{
GridPacker::Ptr GridPacker::createDynamicGrid(Direction dir, unsigned int size,
                                              unsigned int padding) {
    return Ptr(new GridPacker(Dynamic, dir, padding, size));
}

GridPacker::Ptr GridPacker::createFixedGrid(Direction dir, unsigned int wrap,
                                            unsigned int padding) {
    return Ptr(new GridPacker(FixedAmount, dir, padding, wrap));
}

GridPacker::GridPacker(WrapRule r, Direction d, unsigned int pad, unsigned int s)
: rule(r)
, dir(d)
, padding(pad)
, size(s) {}

sf::Vector2i GridPacker::getRequisition(const std::vector<Element::Ptr>& elements) {
    sf::Vector2i req(0, 0);
    auto measure = [&req](Element::Ptr, const sf::IntRect& area) {
        if (req.x < area.left + area.width) req.x = area.left + area.width;
        if (req.y < area.top + area.height) req.y = area.top + area.height;
    };
    doPack({}, elements, measure);
    return req;
}

void GridPacker::pack(const sf::IntRect& area, const std::vector<Element::Ptr>& elements) {
    doPack(area, elements, &Packer::packElementIntoSpace);
}

void GridPacker::doPack(const sf::IntRect& area, const std::vector<Element::Ptr>& elements,
                        const std::function<void(Element::Ptr, const sf::IntRect&)>& packCb) {
    sf::Vector2i pos(area.left, area.top);
    unsigned int pcount = 0;
    sf::Vector2i mreq;

    for (unsigned int i = 0; i < elements.size(); ++i) {
        // Pack
        const sf::Vector2i req = elements[i]->getRequisition();
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
                        pos.x = area.left;
                        pos.y += mreq.y + padding;
                        mreq   = {0, 0};
                        pcount = 0;
                    }
                    else { // coninue in row/column
                        pos.x += req.x + padding;
                    }
                }
                else {
                    const sf::Vector2i nreq = elements[i + 1]->getRequisition();
                    if (pos.x + padding + nreq.x > size) { // wrap
                        pos.x = area.left;
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
                        pos.y = area.top;
                        pos.x += mreq.x + padding;
                        mreq   = {0, 0};
                        pcount = 0;
                    }
                    else { // coninue in row/column
                        pos.y += req.y + padding;
                    }
                }
                else {
                    const sf::Vector2i nreq = elements[i + 1]->getRequisition();
                    if (pos.y + padding + nreq.y > size) { // wrap
                        pos.y = area.top;
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
