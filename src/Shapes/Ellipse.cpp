#include <BLIB/Shapes/Ellipse.hpp>

#include <cmath>

namespace bl
{
Ellipse::Ellipse(float major, float minor, bool co, unsigned int pc)
: nPoints(pc) {
    setAxes(major, minor, co);
}

void Ellipse::setAxes(float major, float minor, bool co) {
    majorRadius = major / 2.f;
    minorRadius = minor / 2.f;
    if (co) setOrigin(majorRadius, minorRadius);
    update();
}

float Ellipse::getMajorAxis() const { return majorRadius * 2.f; }

float Ellipse::getMinorAxis() const { return minorRadius * 2.f; }

std::size_t Ellipse::getPointCount() const { return nPoints; }

sf::Vector2f Ellipse::getPoint(std::size_t index) const {
    static const float pi = 3.141592654f;

    const float ratio = static_cast<float>(index) / static_cast<float>(nPoints);
    const float angle = 2.f * pi * ratio;

    const float x = majorRadius * std::cos(angle);
    const float y = minorRadius * std::sin(angle);

    return {x + majorRadius, y + minorRadius};
}

} // namespace bl