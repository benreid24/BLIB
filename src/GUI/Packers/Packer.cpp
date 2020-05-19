#include <BLIB/GUI/Packers/Packer.hpp>

namespace bl
{
namespace gui
{
void Packer::packElementIntoSpace(Element::Ptr e, const sf::IntRect& space) const {
    const sf::Vector2i size(e->expandsWidth() ? space.width : e->getRequisition().x,
                            e->expandsHeight() ? space.height : e->getRequisition().y);
    const sf::Vector2i pos(space.left + space.width / 2 - size.x / 2,
                           space.top + space.height / 2 - size.y / 2);
    e->assignAcquisition({pos, size});
}

} // namespace gui
} // namespace bl