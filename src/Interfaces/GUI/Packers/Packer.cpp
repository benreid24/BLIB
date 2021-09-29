#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>

namespace bl
{
namespace gui
{
void Packer::packElementIntoSpace(const Element::Ptr& e, const sf::FloatRect& space) {
    const sf::Vector2f size(e->expandsWidth() ? space.width : e->getRequisition().x,
                            e->expandsHeight() ? space.height : e->getRequisition().y);
    const sf::Vector2f pos(space.left + space.width / 2 - size.x / 2,
                           space.top + space.height / 2 - size.y / 2);
    e->assignAcquisition({pos, size});
}

void Packer::manuallyPackElement(const Element::Ptr& e, const sf::FloatRect& area) {
    packElementIntoSpace(e, area);
}

} // namespace gui
} // namespace bl
