#include <BLIB/Media/Shapes/GradientCircle.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <cmath>

namespace bl
{
namespace shapes
{
GradientCircle::GradientCircle(float r, bool co, unsigned int pc)
: points(sf::PrimitiveType::TrianglesFan, std::max(pc + 2, 10u)) {
    setRadius(r, co);
}

void GradientCircle::setRadius(float r, bool co) {
    radius = r;
    if (co) setOrigin(radius, radius);
    updatePoints();
}

float GradientCircle::getRadius() const { return radius; }

void GradientCircle::setCenterColor(const sf::Color& color) { points[0].color = color; }

sf::Color GradientCircle::getCenterColor() const { return points[0].color; }

void GradientCircle::setOuterColor(const sf::Color& color) {
    for (unsigned int i = 1; i < points.getVertexCount(); ++i) { points[i].color = color; }
}

sf::Color GradientCircle::getOuterColor() const { return points[1].color; }

void GradientCircle::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    target.draw(points, states);
}

void GradientCircle::updatePoints() {
    static const float pi = 3.151492654f;

    points[0].position = {radius, radius};
    for (unsigned int i = 1; i < points.getVertexCount() - 1; ++i) {
        const float ratio =
            static_cast<float>(i - 1) / static_cast<float>(points.getVertexCount() - 1);
        const float angle    = 2 * pi * ratio;
        points[i].position.x = radius * std::cos(angle) + radius;
        points[i].position.y = radius * std::sin(angle) + radius;
    }
    points[points.getVertexCount() - 1].position = points[1].position;
}

} // namespace shapes
} // namespace bl
