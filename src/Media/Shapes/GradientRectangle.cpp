#include <BLIB/Media/Shapes/GradientRectangle.hpp>

namespace bl
{
namespace shapes
{
namespace
{
constexpr unsigned int TopLeft     = 0;
constexpr unsigned int TopRight    = 1;
constexpr unsigned int BottomRight = 2;
constexpr unsigned int BottomLeft  = 3;
} // namespace

GradientRectangle::GradientRectangle(const sf::Vector2f& size)
: points(sf::PrimitiveType::Quads, sf::VertexBuffer::Dynamic, 4) {
    points[TopLeft].position     = {0.f, 0.f};
    points[TopRight].position    = {size.x, 0.f};
    points[BottomLeft].position  = {0.f, size.y};
    points[BottomRight].position = size;
}

GradientRectangle::GradientRectangle(const sf::Vector2f& size, Direction gradientDir,
                                     sf::Color fromColor, sf::Color toColor)
: GradientRectangle(size) {
    setGradient(gradientDir, fromColor, toColor);
}

GradientRectangle::GradientRectangle(const sf::Vector2f& size, sf::Color topLeft,
                                     sf::Color topRight, sf::Color bottomLeft,
                                     sf::Color bottomRight)
: GradientRectangle(size) {
    setGradient(topRight, topLeft, bottomLeft, bottomRight);
}

void GradientRectangle::setSize(const sf::Vector2f& size) {
    points[TopLeft].position     = {0.f, 0.f};
    points[TopRight].position    = {size.x, 0.f};
    points[BottomLeft].position  = {0.f, size.y};
    points[BottomRight].position = size;
}

const sf::Vector2f& GradientRectangle::getSize() const { return points[BottomRight].position; }

void GradientRectangle::setGradient(Direction gradientDir, sf::Color fromColor, sf::Color toColor) {
    switch (gradientDir) {
    case TopToBottom:
        points[TopLeft].color     = fromColor;
        points[TopRight].color    = fromColor;
        points[BottomLeft].color  = toColor;
        points[BottomRight].color = toColor;
        break;

    case BottomToTop:
        points[TopLeft].color     = toColor;
        points[TopRight].color    = toColor;
        points[BottomLeft].color  = fromColor;
        points[BottomRight].color = fromColor;
        break;

    case LeftToRight:
        points[TopLeft].color     = fromColor;
        points[BottomLeft].color  = fromColor;
        points[TopRight].color    = toColor;
        points[BottomRight].color = toColor;
        break;

    case RightToLeft:
        points[TopRight].color    = fromColor;
        points[BottomRight].color = fromColor;
        points[TopLeft].color     = toColor;
        points[BottomLeft].color  = toColor;
        break;
    }

    points.update();
}

void GradientRectangle::setGradient(sf::Color topLeft, sf::Color topRight, sf::Color bottomLeft,
                                    sf::Color bottomRight) {
    points[TopLeft].color     = topLeft;
    points[TopRight].color    = topRight;
    points[BottomRight].color = bottomRight;
    points[BottomLeft].color  = bottomLeft;
    points.update();
}

void GradientRectangle::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    target.draw(points, states);
}

} // namespace shapes
} // namespace bl
