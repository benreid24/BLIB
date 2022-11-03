#ifndef BLIB_MATH_VECTOR_HPP
#define BLIB_MATH_VECTOR_HPP

#include <BLIB/Math/Trig.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <cmath>

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
 */
template<typename T>
T magnitude(const sf::Vector2<T>& vec) {
    return std::sqrt(magnitudeSquared(vec));
}

/**
 * @brief Normalizes the given vector in place
 *
 * @tparam T The type of vector components
 * @param vec The vector to normalize
 */
template<typename T>
void normalize(sf::Vector2<T>& vec) {
    const T magSqrd = magnitudeSquared<T>(vec);
    vec.x           = (vec.x * vec.x) / magSqrd;
    vec.y           = (vec.y * vec.y) / magSqrd;
}

/**
 * @brief Computes and returns the normalization of the given vector
 *
 * @tparam T The type of vector components
 * @param vec The vector to normalize
 * @return sf::Vector2<T> The normalized vector
 */
template<typename T>
sf::Vector2<T> normalized(const sf::Vector2<T>& vec) {
    const T magSqrd = magnitudeSquared<T>(vec);
    return {(vec.x * vec.x) / magSqrd, (vec.y * vec.y) / magSqrd};
}

/**
 * @brief Computes the dot product of the two given vectors
 *
 * @tparam T The type of vector components
 * @param v1 The left vector
 * @param v2 The right vector
 * @return T The dot product of v1 and v2
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
 */
template<typename T>
T magnitude(const sf::Vector3<T>& vec) {
    return std::sqrt(magnitudeSquared(vec));
}

/**
 * @brief Normalizes the given vector in place
 *
 * @tparam T The type of vector components
 * @param vec The vector to normalize
 */
template<typename T>
void normalize(sf::Vector3<T>& vec) {
    const T magSqrd = magnitudeSquared<T>(vec);
    vec.x           = (vec.x * vec.x) / magSqrd;
    vec.y           = (vec.y * vec.y) / magSqrd;
    vec.z           = (vec.z * vec.z) / magSqrd;
}

/**
 * @brief Computes and returns the normalization of the given vector
 *
 * @tparam T The type of vector components
 * @param vec The vector to normalize
 * @return sf::Vector3<T> The normalized vector
 */
template<typename T>
sf::Vector3<T> normalized(const sf::Vector3<T>& vec) {
    const T magSqrd = magnitudeSquared<T>(vec);
    return {(vec.x * vec.x) / magSqrd, (vec.y * vec.y) / magSqrd, (vec.z * vec.z) / magSqrd};
}

/**
 * @brief Computes the dot product of the two given vectors
 *
 * @tparam T The type of vector components
 * @param v1 The left vector
 * @param v2 The right vector
 * @return T The dot product of v1 and v2
 */
template<typename T>
T dot(const sf::Vector3<T>& v1, const sf::Vector3<T>& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

} // namespace math
} // namespace bl

#endif
