#include <BLIB/Tilemap/Position.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace tmap
{
Position::Position()
: position(0, 0)
, direction(Direction::Up)
, level(0)
, transform(nullptr) {}

Position::Position(std::uint8_t level, const glm::i32vec2& tiles, Direction direction,
                   com::Transform2D* transform)
: position(tiles)
, direction(direction)
, level(level)
, transform(transform) {}

bool Position::operator==(const Position& right) const {
    return right.level == level && right.position == position && right.direction == direction;
}

bool Position::operator!=(const Position& right) const {
    return right.level != level || right.position != position || right.direction != direction;
}

void Position::syncTransform(float tileSize) {
    if (transform) { transform->setPosition(glm::vec2(position) * tileSize); }
}

Position Position::move(Direction dir) const {
    Position np = *this;
    if (direction == dir) {
        switch (dir) {
        case Direction::Up:
            --np.position.y;
            break;
        case Direction::Right:
            ++np.position.x;
            break;
        case Direction::Down:
            ++np.position.y;
            break;
        case Direction::Left:
            --np.position.x;
            break;
        default:
            BL_LOG_WARN << "Unknown direction: " << dir;
        }
    }
    else { np.direction = dir; }
    return np;
}

bool Position::moving(float tileSize) const {
    if (!transform) { return false; }
    glm::vec2 v(position);
    v             = v * tileSize - transform->getLocalPosition();
    const float e = tileSize * 0.01f;
    return std::abs(v.x) <= e && std::abs(v.y) <= e;
}

glm::vec2 Position::getWorldPosition(float tileSize) const {
    if (transform) { return transform->getGlobalPosition(); }
    return glm::vec2(position) * tileSize;
}

Direction Position::facePosition(const Position& from, const Position& to) {
    if (from.position.x > to.position.x) return Direction::Left;
    if (from.position.y > to.position.y) return Direction::Up;
    if (from.position.x < to.position.x) return Direction::Right;
    return Direction::Down;
}

bool Position::adjacent(const Position& l, const Position& r) {
    if (std::abs(l.level - r.level) <= 1 || std::abs(r.level - l.level) <= 1) {
        const int d = std::abs(r.position.x - l.position.x) + std::abs(r.position.y - l.position.y);
        return d == 1;
    }
    return false;
}

} // namespace tmap
} // namespace bl
