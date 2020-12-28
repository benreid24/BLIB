#ifndef BLIB_UTIL_ANGULARVECTOR_HPP
#define BLIB_UTILANGULARVECTOR_HPP

#include <SFML/Graphics.hpp>
#include <cmath>

namespace bg
{
/**
 * @brief Angular contemporary to sf::Vector2<T>.
 *        The angle is consistent with SFML's rendering reference frame. 0 degrees is straight
 *        up
 */
template<typename T>
struct AngularVector {
    T magnitude;
    T angle;

    static constexpr StdToSFMLRotationOffset = 90;

    /**
     * @brief Creates a vector with 0 length at 0 degrees
     *
     */
    AngularVector()
    : magnitude(0)
    , angle(0) {}

    /**
     * @brief Creates a vector with the given magnitude and direction
     *
     * @param magnitude The length of the vector
     * @param angle The direction, in degrees
     */
    AngularVector(T magnitude, T angle)
    : magnitude(magnitude)
    , angle(angle) {}

    /**
     * @brief Copy constructor
     *
     */
    AngularVector(const AngularVector& copy) = default;

    /**
     * @brief Construct from a cartesian vector
     *
     * @param cartesianVector The vector to convert to angular corrdinates
     */
    AngularVector(const sf::Vector2<T>& cartesianVector)
    : magnitude(std::sqrt(cartesianVector.x * cartesianVector.x +
                          cartesianVector.y * cartesianVector.y))
    , angle(std::atan2(cartesianVector.y, cartesianVector.x) * 180 / 3.1415926 +
            StdToSFMLRotationOffset) {}

    /**
     * @brief Convert the vector to Cartesian coordinates
     *
     * @return sf::Vector2<T> The resulting Cartesian vesctor
     */
    sf::Vector2<T> toCartesian() const {
        const T adjAngle = angle - StdToSFMLRotationOffset;
        return {magnitude * static_cast<T>(std::cos(adjAngle / 180 * 3.1415926)),
                magnitude * static_cast<T>(std::sin(adjAngle / 180 * 3.1415926))};
    }

    /**
     * @brief Apply a rotation to the angle of the vector
     *
     * @param d The angle to rotate by, in degrees
     * @return AngularVector& Reference to this vector. Useful for chaining transforms
     */
    AngularVector& rotate(T d) {
        angle += d;
        return *this;
    }

    AngularVector& operator=(const AngularVector&) = default;
    void operator+=(const AngularVector& v) { *this = *this + v; }
    void operator-=(const AngularVector& v) { *this = *this - v; }
    void operator+=(const sf::Vector2<T>& v) { *this = *this + v; }
    void operator-=(const sf::Vector2<T>& v) { *this = *this - v; }
    void operator*=(T m) { magnitude *= m; }
    void operator/=(T m) { magnitude /= m; }

    /**
     * @brief Add an AngularVector to this vector
     *
     */
    AngularVector operator+(const AngularVector& v) {
        return AngularVector(toCartesian() + v.toCartesian());
    }

    /**
     * @brief Add a Cartesian vector to this AngularVector
     *
     */
    AngularVector operator+(const sf::Vector2<T>& v) {
        *this = AngularVector(toCartesian() + v);
        return *this;
    }

    /**
     * @brief Subtract an AngularVector from this vector
     *
     */
    AngularVector operator-(const AngularVector& v) {
        return AngularVector(toCartesian() - v.toCartesian());
    }

    /**
     * @brief Subtract a Cartesian vector from this vector
     *
     */
    AngularVector operator-(const sf::Vector2<T>& v) {
        return AngularVector(toCartesian() - v);
    }

    /**
     * @brief Scale the vector magnitude with a scalar
     *
     */
    AngularVector operator*(T m) { return {magnitude * m, angle}; }

    /**
     * @brief Scale the vector magnitude with a scalar
     *
     */
    AngularVector operator/(T m) { return {magnitude / m, angle}; }
};

typedef AngularVector<float> AngularVectorF;

} // namespace bg

#endif