#ifndef BLIB_MATH_VECTOR_HPP
#define BLIB_MATH_VECTOR_HPP

#include <BLIB/Math/Trig.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <cmath>
#include <glm/glm.hpp>

namespace bl
{
namespace math
{
/**
 * @brief Computes the squared magnitude of the given vector
 *
 * @tparam T The type of vector components
 * @param vec The vector to get the squared magnitude of
 * @return T The squared magnitude of the vector
 *
 * @ingroup Math
 */
template<typename T>
T magnitudeSquared(const sf::Vector2<T>& vec) {
    return vec.x * vec.x + vec.y * vec.y;
}

/**
 * @brief Computes the magnitude of the given vector. Uses std::sqrt, prefer squaredMagnitude
 *
 * @tparam T The type of vector components
 * @param vec The vector to get the magnitude of
 * @return T The magnitude of the vector
 *
 * @ingroup Math
 */
template<typename T>
T magnitude(const sf::Vector2<T>& vec) {
    return std::sqrt(magnitudeSquared(vec));
}

/**
 * @brief Returns the squared distance between the two vectors
 *
 * @tparam T The type of vector components
 * @param v1 The first vector
 * @param v2 The second vector
 * @return T The square of the distance between them
 *
 * @ingroup Math
 */
template<typename T>
T distanceSquared(const sf::Vector2<T>& v1, const sf::Vector2<T>& v2) {
    return magnitudeSquared(v1 - v2);
}

/**
 * @brief Returns the distance between two vectors
 *
 * @tparam T The type of vector components
 * @param v1 The first vector
 * @param v2 The second vector
 * @return T The distance between them
 *
 * @ingroup Math
 */
template<typename T>
T distance(const sf::Vector2<T>& v1, const sf::Vector2<T>& v2) {
    return std::sqrt(distanceSquared(v1, v2));
}

/**
 * @brief Returns the manhattan distance between two vectors
 *
 * @tparam T The type of vector components
 * @param v1 The first vector
 * @param v2 The second vector
 * @return T The manhattan distance between them
 *
 * @ingroup Math
 */
template<typename T>
T manhattanDistance(const sf::Vector2<T>& v1, const sf::Vector2<T>& v2) {
    return std::abs(v1.x - v2.x) + std::abs(v1.y - v2.y);
}

/**
 * @brief Normalizes the given vector in place
 *
 * @tparam T The type of vector components
 * @param vec The vector to normalize
 *
 * @ingroup Math
 */
template<typename T>
void normalize(sf::Vector2<T>& vec) {
    const T mag = magnitude<T>(vec);
    vec.x /= mag;
    vec.y /= mag;
}

/**
 * @brief Computes and returns the normalization of the given vector
 *
 * @tparam T The type of vector components
 * @param vec The vector to normalize
 * @return sf::Vector2<T> The normalized vector
 *
 * @ingroup Math
 */
template<typename T>
sf::Vector2<T> normalized(const sf::Vector2<T>& vec) {
    const T mag = magnitude<T>(vec);
    return {vec.x / mag, vec.y / mag};
}

/**
 * @brief Computes the dot product of the two given vectors
 *
 * @tparam T The type of vector components
 * @param v1 The left vector
 * @param v2 The right vector
 * @return T The dot product of v1 and v2
 *
 * @ingroup Math
 */
template<typename T>
T dot(const sf::Vector2<T>& v1, const sf::Vector2<T>& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

/**
 * @brief Computes the angle that the given vector is pointing
 *
 * @tparam T The type of vector components
 * @param vec The vector to get the angle of
 * @return T The angle the vector is pointing
 *
 * @ingroup Math
 */
template<typename T>
T vectorToAngle(const sf::Vector2<T>& vec) {
    return radiansToDegrees(std::atan2(vec.y, vec.x));
}

/**
 * @brief Returns a unit vector for the given angle. Uses the lookup table trig functions
 *
 * @tparam TThe type of vector components
 * @param degrees The angle to convert, in degrees
 * @return sf::Vector2<T> The unit vector facing the given angle
 *
 * @ingroup Math
 */
template<typename T>
sf::Vector2<T> angleToVector(T degrees) {
    return sf::Vector2<T>{cos(degrees), sin(degrees)};
}

/**
 * @brief Computes the squared magnitude of the given vector
 *
 * @tparam T The type of vector components
 * @param vec The vector to get the squared magnitude of
 * @return T The squared magnitude of the vector
 *
 * @ingroup Math
 */
template<typename T>
T magnitudeSquared(const sf::Vector3<T>& vec) {
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

/**
 * @brief Computes the magnitude of the given vector. Uses std::sqrt, prefer squaredMagnitude
 *
 * @tparam T The type of vector components
 * @param vec The vector to get the magnitude of
 * @return T The magnitude of the vector
 *
 * @ingroup Math
 */
template<typename T>
T magnitude(const sf::Vector3<T>& vec) {
    return std::sqrt(magnitudeSquared(vec));
}

/**
 * @brief Returns the squared distance between the two vectors
 *
 * @tparam T The type of vector components
 * @param v1 The first vector
 * @param v2 The second vector
 * @return T The square of the distance between them
 *
 * @ingroup Math
 */
template<typename T>
T distanceSquared(const sf::Vector3<T>& v1, const sf::Vector3<T>& v2) {
    return magnitudeSquared(v1 - v2);
}

/**
 * @brief Returns the distance between two vectors
 *
 * @tparam T The type of vector components
 * @param v1 The first vector
 * @param v2 The second vector
 * @return T The distance between them
 *
 * @ingroup Math
 */
template<typename T>
T distance(const sf::Vector3<T>& v1, const sf::Vector3<T>& v2) {
    return std::sqrt(distanceSquared(v1, v2));
}

/**
 * @brief Returns the manhattan distance between two vectors
 *
 * @tparam T The type of vector components
 * @param v1 The first vector
 * @param v2 The second vector
 * @return T The manhattan distance between them
 *
 * @ingroup Math
 */
template<typename T>
T manhattanDistance(const sf::Vector3<T>& v1, const sf::Vector3<T>& v2) {
    return std::abs(v1.x - v2.x) + std::abs(v1.y - v2.y) + std::abs(v1.z - v2.z);
}

/**
 * @brief Normalizes the given vector in place
 *
 * @tparam T The type of vector components
 * @param vec The vector to normalize
 *
 * @ingroup Math
 */
template<typename T>
void normalize(sf::Vector3<T>& vec) {
    const T mag = magnitude<T>(vec);
    vec.x /= mag;
    vec.y /= mag;
    vec.z /= mag;
}

/**
 * @brief Computes and returns the normalization of the given vector
 *
 * @tparam T The type of vector components
 * @param vec The vector to normalize
 * @return sf::Vector3<T> The normalized vector
 *
 * @ingroup Math
 */
template<typename T>
sf::Vector3<T> normalized(const sf::Vector3<T>& vec) {
    const T mag = magnitude<T>(vec);
    return {vec.x / mag, vec.y / mag, vec.z / mag};
}

/**
 * @brief Computes the dot product of the two given vectors
 *
 * @tparam T The type of vector components
 * @param v1 The left vector
 * @param v2 The right vector
 * @return T The dot product of v1 and v2
 *
 * @ingroup Math
 */
template<typename T>
T dot(const sf::Vector3<T>& v1, const sf::Vector3<T>& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

/**
 * @brief Computes the angle pointing from one vector to another
 *
 * @param from The start position
 * @param to The end position
 * @return The angle in degrees in range [0, 360)
 *
 * @ingroup Math
 */
inline float computeAngle(const glm::vec2& from, const glm::vec2& to) {
    const glm::vec2 diff = to - from;
    float angle          = bl::math::radiansToDegrees(::atan2f(diff.y, diff.x));
    if (angle < 0.f) { angle += 360.f; }
    return angle;
}

} // namespace math
} // namespace bl

#endif
