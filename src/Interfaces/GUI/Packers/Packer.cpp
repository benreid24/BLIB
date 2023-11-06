#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>

namespace bl
{
namespace gui
{
void Packer::packElementIntoSpace(const Element::Ptr& e, const sf::FloatRect& space) {
    const sf::Vector2f totalSize(space.width, space.height);
    const sf::Vector2f size(e->expandsWidth() ? totalSize : e->getRequisition());
    const sf::Vector2f pos(space.left + space.width * 0.5f - size.x * 0.5f,
                           space.top + space.height * 0.5f - size.y * 0.5f);
    e->assignAcquisition({pos, size});
}

void Packer::manuallyPackElement(const Element::Ptr& e, const sf::FloatRect& area, bool f) {
    if (f) { e->assignAcquisition(area); }
    else { packElementIntoSpace(e, area); }
}

} // namespace gui
} // namespace bl
