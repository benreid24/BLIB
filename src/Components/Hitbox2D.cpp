#include <BLIB/Components/Hitbox2D.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace com
{
namespace
{
bool circleIntersectsRect(const glm::vec2& circleCenter, float radius, const glm::vec2& rectCenter,
                          const glm::vec2& size) {
    const glm::vec2 halfSize   = size * 0.5f;
    const glm::vec2 centerDist = glm::abs(circleCenter - rectCenter);

    if (centerDist.x > halfSize.x + radius || centerDist.y > halfSize.y + radius) { return false; }
    if (centerDist.x <= halfSize.x || centerDist.x <= halfSize.y) { return true; }

    const glm::vec2 cornerDist = centerDist - halfSize;
    return (cornerDist.x * cornerDist.x + cornerDist.y * cornerDist.y) <= radius * radius;
}
} // namespace

Hitbox2D::Hitbox2D(Transform2D* transform)
: transform(transform)
, type(Point) {}

com::Hitbox2D::Hitbox2D(Transform2D* transform, float radius)
: transform(transform)
, type(Circle)
, radius(radius) {}

Hitbox2D::Hitbox2D(Transform2D* transform, const glm::vec2& size)
: transform(transform)
, type(Rectangle)
, size(size) {}

bool Hitbox2D::intersects(const Hitbox2D& other) const {
    switch (other.type) {
    case Point:
        return containsPoint(other.transform->getGlobalPosition() + other.transform->getOrigin());
    case Circle:
        return intersectsCircle(other.transform->getGlobalPosition(), other.radius);
    case Rectangle:
        return intersectsRect(other.transform->getGlobalPosition() - other.transform->getOrigin(),
                              other.size);
    default:
        return false;
    }
}

bool Hitbox2D::containsPoint(const glm::vec2& point) const {
    switch (type) {
    case Circle: {
        const glm::vec2 d = transform->getGlobalPosition() - point;
        return (d.x * d.x + d.y * d.y) <= radius * radius;
    }
    case Rectangle: {
        const glm::vec2 lp = point - (transform->getGlobalPosition() - transform->getOrigin());
        return lp.x >= 0.f && lp.y >= 0.f && lp.x <= size.x && lp.y <= size.y;
    }
    case Point:
    default:
        return false;
    }
}

bool Hitbox2D::intersectsCircle(const glm::vec2& center, float otherRadius) const {
    switch (type) {
    case Circle: {
        const glm::vec2 d = transform->getGlobalPosition() - center;
        const float r     = radius + otherRadius;
        return (d.x * d.x + d.y * d.y) <= (r * r);
    }
    case Rectangle: {
        return circleIntersectsRect(center,
                                    otherRadius,
                                    transform->getGlobalPosition() - transform->getOrigin() +
                                        size * 0.5f,
                                    size);
    }
    case Point: {
        const glm::vec2 d = transform->getGlobalPosition() + transform->getOrigin() - center;
        return (d.x * d.x + d.y * d.y) <= otherRadius * otherRadius;
    }
    default:
        return false;
    }
}

bool Hitbox2D::intersectsRect(const glm::vec2& corner, const glm::vec2& otherSize) const {
    switch (type) {
    case Circle: {
        return circleIntersectsRect(
            transform->getGlobalPosition(), radius, corner + otherSize * 0.5f, otherSize);
    }
    case Rectangle: {
        const glm::vec2 myCorner       = transform->getGlobalPosition() - transform->getOrigin();
        const glm::vec2 myOtherSide    = myCorner + size;
        const glm::vec2 theirOtherSide = corner + otherSize;
        const float interLeft          = std::max(myCorner.x, corner.x);
        const float interTop           = std::max(myCorner.y, corner.y);
        const float interRight         = std::min(myOtherSide.x, theirOtherSide.x);
        const float interBottom        = std::min(myOtherSide.y, theirOtherSide.y);
        return interLeft < interRight && interTop < interBottom;
    }
    case Point: {
        const glm::vec2 lp = (transform->getGlobalPosition() - transform->getOrigin()) - corner;
        return lp.x >= 0.f && lp.y >= 0.f && lp.x <= otherSize.x && lp.y <= otherSize.y;
    }
    default:
        return false;
    }
}

void Hitbox2D::makePoint() { type = Point; }

void Hitbox2D::makeCircle(float r) {
    type   = Circle;
    radius = r;
}

void Hitbox2D::makeRect(const glm::vec2& s) {
    type = Rectangle;
    size = s;
}

} // namespace com
} // namespace bl
