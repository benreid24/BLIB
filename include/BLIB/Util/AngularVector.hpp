#ifndef BLIB_UTIL_ANGULARVECTOR_HPP
#define BLIB_UTILANGULARVECTOR_HPP

#include <SFML/Graphics.hpp>
#include <cmath>

namespace bl
{
/**
 * @brief Angular contemporary to sf::Vector2<T>
 */
template<typename T>
struct AngularVector {
    T magnitude;
    T angle;

    AngularVector()
    : magnitude(0)
    , angle(0) {}
    AngularVector(T magnitude, T angle)
    : magnitude(magnitude)
    , angle(angle) {}
    AngularVector(const AngularVector& copy) = default;
    AngularVector(const sf::Vector2<T>& cartesianVector)
    : magnitude(std::sqrt(cartesianVector.x * cartesianVector.x +
                          cartesianVector.y * cartesianVector.y))
    , angle(std::atan2(cartesianVector.y, cartesianVector.x) * 180 / 3.1415926 +
            Properties::StdToSFMLRotationOffset) {}

    sf::Vector2<T> toCartesian() const {
        const T adjAngle = angle - Properties::StdToSFMLRotationOffset;
        return {magnitude * static_cast<T>(std::cos(adjAngle / 180 * 3.1415926)),
                magnitude * static_cast<T>(std::sin(adjAngle / 180 * 3.1415926))};
    }

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

    AngularVector operator+(const AngularVector& v) {
        return AngularVector(toCartesian() + v.toCartesian());
    }
    AngularVector operator+(const sf::Vector2<T>& v) {
        *this = AngularVector(toCartesian() + v);
        return *this;
    }

    AngularVector operator-(const AngularVector& v) {
        return AngularVector(toCartesian() - v.toCartesian());
    }
    AngularVector operator-(const sf::Vector2<T>& v) {
        return AngularVector(toCartesian() - v);
    }

    AngularVector operator*(T m) { return {magnitude * m, angle}; }
    AngularVector operator/(T m) { return {magnitude / m, angle}; }
};

typedef AngularVector<float> AngularVectorF;

} // namespace bl

#endif