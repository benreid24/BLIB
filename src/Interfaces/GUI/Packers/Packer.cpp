#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>

namespace bl
{
namespace gui
{
void Packer::packElementIntoSpace(const Element::Ptr& e, const sf::FloatRect& space) {
    const sf::Vector2f totalSize(space.size.x, space.size.y);
    const sf::Vector2f size(
        e->expandsWidth() ? totalSize.x : std::min(e->getRequisition().x, totalSize.x),
        e->expandsHeight() ? totalSize.y : std::min(e->getRequisition().y, totalSize.y));
    const sf::Vector2f pos(space.position.x + space.size.x * 0.5f - size.x * 0.5f,
                           space.position.y + space.size.y * 0.5f - size.y * 0.5f);
    e->assignAcquisition({pos, size});
}

void Packer::manuallyPackElement(const Element::Ptr& e, const sf::FloatRect& area, bool f) {
    if (f) { e->assignAcquisition(area); }
    else { packElementIntoSpace(e, area); }
}

} // namespace gui
} // namespace bl
