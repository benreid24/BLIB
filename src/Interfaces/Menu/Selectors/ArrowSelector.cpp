#include <BLIB/Interfaces/Menu/Selectors/ArrowSelector.hpp>

namespace bl
{
namespace menu
{
namespace
{
const float HeightRatio = 0.5f;

std::array<glm::vec2, 3> makeTriangle(float w) {
    return std::array<glm::vec2, 3>(
        {glm::vec2{0.f, 0.f}, glm::vec2{w, w * HeightRatio}, glm::vec2{0, 2.f * w * HeightRatio}});
}
} // namespace

ArrowSelector::Ptr ArrowSelector::create(float w, const sf::Color& f) {
    return Ptr(new ArrowSelector(w, f));
}

ArrowSelector::ArrowSelector(float w, const sf::Color& f)
: width(w)
, triangle() {
    // triangle.setFillColor(f);
    // TODO - create triangle. sf::Color helper
}

gfx::Triangle& ArrowSelector::getArrow() { return triangle; }

void ArrowSelector::render(sf::RenderTarget& target, sf::RenderStates states,
                           sf::FloatRect itemArea) const {
    /* triangle.setPosition({itemArea.left - width - 2.f,
                          itemArea.top + itemArea.height / 2.f + width * HeightRatio / 2.f});
    target.draw(triangle, states);*/
    // TODO - set position earlier.
}

} // namespace menu
} // namespace bl
