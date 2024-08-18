#ifndef BLIB_MATH_TRIG_HPP
#define BLIB_MATH_TRIG_HPP

#include <BLIB/Math/Constants.hpp>

namespace bl
{
namespace math
{
/**
 * @brief Computes the cosine of the given angle using cached values. Values are cached in quarter
 *        degree increments
 *
 * @param degrees The angle in degrees
 * @return float The cosine of the angle
 *
 * @ingroup Math
 */
float cos(float degrees);

/**
 * @brief Computes the sine of the given angle using cached values. Values are cached in quarter
 *        degree increments
 *
 * @param degrees The angle in degrees
 * @return float The sine of the angle
 *
 * @ingroup Math
 */
float sin(float degrees);

/**
 * @brief Converts an angle in radians to degrees
 *
 * @param radians The angle in radians
 * @return float The angle in degrees
 *
 * @ingroup Math
 */
inline constexpr float radiansToDegrees(float radians) { return radians * 180.f / Pi; }

/**
 * @brief Converts an angle in degrees to radians
 *
 * @param radians The angle in degrees
 * @return float The angle in radians
 *
 * @ingroup Math
 */
inline constexpr float degreesToRadians(float degrees) { return degrees / 180.f * Pi; }

} // namespace math
} // namespace bl

#endif
