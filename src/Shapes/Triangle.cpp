#include <BLIB/Shapes/Triangle.hpp>

namespace bl
{
Triangle::Triangle(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3,
                   bool co) {
    setPoints(p1, p2, p3, co);
}

void Triangle::setPoints(const sf::Vector2f& p1, const sf::Vector2f& p2,
                         const sf::Vector2f& p3, bool co) {
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
    if (co) setOrigin((p1.x + p2.x + p3.x) / 3, (p1.y + p2.y + p3.y) / 3);
    update();
}

std::size_t Triangle::getPointCount() const { return 3; }

sf::Vector2f Triangle::getPoint(size_t index) const { return points[index]; }

} // namespace bl