#include <BLIB/Interfaces/Menu/Selectors/ArrowSelector.hpp>

namespace bl
{
namespace menu
{
namespace
{
const float HeightRatio = 0.5;

bl::shapes::Triangle makeTriangle(float w) {
    return bl::shapes::Triangle({0, 0}, {w, w * HeightRatio}, {0, 2.f * w * HeightRatio});
}
} // namespace

ArrowSelector::Ptr ArrowSelector::create(float w, const sf::Color& f) {
    return Ptr(new ArrowSelector(w, f));
}

ArrowSelector::ArrowSelector(float w, const sf::Color& f)
: width(w)
, triangle(makeTriangle(w)) {
    triangle.setFillColor(f);
}

shapes::Triangle& ArrowSelector::getArrow() { return triangle; }

void ArrowSelector::render(sf::RenderTarget& target, sf::RenderStates states,
                           sf::FloatRect itemArea) const {
    triangle.setPosition({itemArea.left - width - 2.f,
                          itemArea.top + itemArea.height / 2.f + width * HeightRatio / 2.f});
    target.draw(triangle, states);
}

} // namespace menu
} // namespace bl
