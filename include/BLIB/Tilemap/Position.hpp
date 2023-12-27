#ifndef BLIB_TILEMAP_POSITION_HPP
#define BLIB_TILEMAP_POSITION_HPP

#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/Serialization.hpp>
#include <BLIB/Tilemap/Direction.hpp>
#include <glm/glm.hpp>

namespace bl
{
/// Tilemap classes and functionality
namespace tmap
{
class Position {
public:
    /**
     * @brief Creates an empty position
     *
     */
    Position();

    /**
     * @brief Construct a new Position component
     *
     * @param level The level in the map
     * @param tiles The position in tiles
     * @param direction The direction it is facing
     * @param transform The world transform component for this position
     */
    Position(std::uint8_t level, const glm::i32vec2& tiles, Direction direction,
             com::Transform2D* transform = nullptr);

    /**
     * @brief Tests whether this position is equal to another. Does not consider interpolation or
     *        direction, only tiles and level
     *
     * @param other The position to test against
     * @return True if the positions are on the same level and tile, false otherwise
     */
    bool operator==(const Position& other) const;

    /**
     * @brief Tests whether this position is not equal to another. Does not consider interpolation
     *        or direction, only tiles and level
     *
     * @param other The position to test against
     * @return True if the positions are not on the same level or tile, false otherwise
     */
    bool operator!=(const Position& other) const;

    /**
     * @brief Sets the world transform offset to the correct position
     *
     * @param tileSize The size, in world units, of each map tile
     */
    void syncTransform(float tileSize);

    /**
     * @brief Returns the position adjacent to this one when moving in the given direction.
     *        Does not handle level transition or interpolation. If the direction moved is not the
     *        same as the current direction then no movement occurs and the direction is changed
     *
     * @param dir The direction to move in
     * @return Position The new position after moving or rotating
     */
    Position move(Direction dir) const;

    /**
     * @brief Returns whether or not this position is currently being interpolated. Always returns
     *        false if the world transform is nullptr
     *
     * @param tileSize The size, in world units, of each map tile
     * @return True if there is active unfinished interpolation, false if still
     */
    bool moving(float tileSize) const;

    /**
     * @brief Returns the direction that an entity at 'from' position should face to face the 'to'
     *        position
     *
     * @param from The position of the entity being rotated
     * @param to The position to face
     * @return Direction The direction to face
     */
    static Direction facePosition(const Position& from, const Position& to);

    /**
     * @brief Tests whether two positions are adjacent to one another and within one level
     *
     * @param left One position to test with
     * @param right The other position to test with
     * @return True if the positions are adjacent, false if not
     */
    static bool adjacent(const Position& left, const Position& right);

    // The coordinate of the position on the map
    glm::i32vec2 position;

    /// The direction the entity is facing
    Direction direction;

    /// The level of the map that the entity is on
    std::uint8_t level;

    /// Optional world transform component for this position
    com::Transform2D* transform;
};

} // namespace tmap

namespace serial
{
namespace binary
{
template<>
struct Serializer<tmap::Position, false> {
    static bool serialize(OutputStream& output, const tmap::Position& position) {
        if (!Serializer<glm::i32vec2>::serialize(output, position.position)) return false;
        if (!Serializer<tmap::Direction>::serialize(output, position.direction)) { return false; }
        return Serializer<std::uint8_t>::serialize(output, position.level);
    }

    static bool deserialize(InputStream& input, tmap::Position& result) {
        if (!Serializer<glm::i32vec2>::deserialize(input, result.position)) return false;
        if (!Serializer<tmap::Direction>::deserialize(input, result.direction)) { return false; }
        return Serializer<std::uint8_t>::deserialize(input, result.level);
    }

    static std::uint32_t size(const tmap::Position& pos) {
        return Serializer<glm::i32vec2>::size(pos.position) +
               Serializer<tmap::Direction>::size(pos.direction) +
               Serializer<std::uint8_t>::size(pos.level);
    }
};

} // namespace binary

template<>
struct SerializableObject<tmap::Position> : SerializableObjectBase {
    using Pos = tmap::Position;
    using Dir = tmap::Direction;

    SerializableField<1, Pos, std::uint8_t> level;
    SerializableField<2, Pos, Dir> direction;
    SerializableField<3, Pos, glm::i32vec2> position;

    SerializableObject()
    : SerializableObjectBase("Position")
    , level("level", *this, &Pos::level, SerializableFieldBase::Required{})
    , direction("direction", *this, &Pos::direction, SerializableFieldBase::Required{})
    , position("position", *this, &Pos::position, SerializableFieldBase::Required{}) {}
};

} // namespace serial

} // namespace bl

#endif
