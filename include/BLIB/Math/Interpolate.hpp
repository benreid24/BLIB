#ifndef BLIB_MATH_INTERPOLATE_HPP
#define BLIB_MATH_INTERPOLATE_HPP

namespace bl
{
namespace math
{
/**
 * @brief Linearly interpolate a value from one range to another
 *
 * @tparam T The type to interpolate
 * @param originStart The beginning of the range the value is in
 * @param originEnd The end of the range the value is in
 * @param destStart The beginning of the range to interpolate to
 * @param destEnd  The end of the range to interpolate to
 * @param value The value to interpolate
 * @return T The interpolated value
 *
 *  @ingroup Math
 */
template<typename T>
T interpolate(T originStart, T originEnd, T destStart, T destEnd, T value) {
    const T fr = originEnd - originStart;
    const T dr = destEnd - destStart;
    return (value - originStart) / fr * dr + destStart;
}

/**
 * @brief Linearly interpolate a value from one range to another with a midpoint on the initial
 *        range and an assumed centered endpoint on the end range
 *
 * @tparam T The type to interpolate
 * @param originStart The beginning of the range the value is in
 * @param originMid The middle of the range the value is in
 * @param originEnd The end of the range the value is in
 * @param destStart The beginning of the range to interpolate to
 * @param destEnd  The end of the range to interpolate to
 * @param value The value to interpolate
 * @return T The interpolated value
 *
 *  @ingroup Math
 */
template<typename T>
T interpolate(T originStart, T originMid, T originEnd, T destStart, T destEnd, T value) {
    const T destMid = (destEnd + destStart) / static_cast<T>(2);
    return value <= originMid ? interpolate(originStart, originMid, destStart, destMid, value) :
                                interpolate(originMid, originEnd, destMid, destEnd, value);
}

/**
 * @brief Linearly interpolate a value from one range to another with a midpoint on both the initial
 *        range and the final range
 *
 * @tparam T The type to interpolate
 * @param originStart The beginning of the range the value is in
 * @param originMid The middle of the range the value is in
 * @param originEnd The end of the range the value is in
 * @param destStart The beginning of the range to interpolate to
 * @param destMid The middle of the range to interpolate to
 * @param destEnd  The end of the range to interpolate to
 * @param value The value to interpolate
 * @return T The interpolated value
 *
 *  @ingroup Math
 */
template<typename T>
T interpolate(T originStart, T originMid, T originEnd, T destStart, T destMid, T destEnd, T value) {
    return value <= originMid ? interpolate(originStart, originMid, destStart, destMid, value) :
                                interpolate(originMid, originEnd, destMid, destEnd, value);
}

} // namespace math
} // namespace bl

#endif
