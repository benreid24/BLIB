#ifndef BLIB_MATH_RANGE_HPP
#define BLIB_MATH_RANGE_HPP

#include <algorithm>

namespace bl
{
namespace math
{
/**
 * @brief Helper struct to define a range of numerical values
 *
 * @tparam T The type of value to represent
 * @ingroup Math
 */
template<typename T>
struct Range {
    T min;
    T max;

    /**
     * @brief Creates an empty range
     */
    Range()
    : min(T{})
    , max(T{}) {}

    /**
     * @brief Creates a range
     *
     * @param min The minimum value of the range
     * @param max The maximum value of the range
     */
    Range(T min, T max)
    : min(min)
    , max(max) {}

    /**
     * @brief Returns the width of the range
     */
    T width() const { return max - min; }

    /**
     * @brief Clamps the given value to be within the range
     *
     * @param value The value to clamp
     * @return
     */
    T clamp(T value) const {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    /**
     * @brief Tests whether a value is within the range
     *
     * @param value The value to test
     * @return True if the value is within the range, false otherwise
     */
    bool contains(T value) const { return value >= min && value <= max; }

    /**
     * @brief Linearly interpolates between the min and max values of the range
     *
     * @param t The lerp factor
     * @return The interpolated value
     */
    T lerp(float t) const { return min + (max - min) * t; }

    /**
     * @brief Performs the inverse of a linear interpolation, returning the lerp factor for a value
     *
     * @param value The value to inverse lerp
     * @return The lerp factor for the value
     */
    T inverseLerp(T value) const {
        if (max == min) return 0.f;
        return (value - min) / (max - min);
    }

    /**
     * @brief Maps a value from this range to another range
     *
     * @param value The value to map
     * @param other The other range to map to
     * @return The mapped value
     */
    T map(T value, const Range<T>& other) const { return other.lerp(inverseLerp(value)); }

    /**
     * @brief Returns the overlap between this range and another range
     *
     * @param other The other range to test against
     * @return The amount of overlap between the two ranges. 0 if they do not overlap
     */
    T overlap(const Range<T>& other) const {
        return std::max(T(0), std::min(max, other.max) - std::max(min, other.min));
    }

    /**
     * @brief Tests whether this range overlaps with another range
     *
     * @param other The range to test against
     * @return True if the ranges overlap, false otherwise
     */
    bool overlaps(const Range<T>& other) const {
        return std::max(min, other.min) <= std::min(max, other.max);
    }
};

} // namespace math
} // namespace bl

#endif
