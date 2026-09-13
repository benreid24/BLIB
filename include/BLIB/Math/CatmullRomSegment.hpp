#ifndef BLIB_MATH_CATMULLROMSEGMENT_HPP
#define BLIB_MATH_CATMULLROMSEGMENT_HPP

#include <glm/glm.hpp>

namespace bl
{
namespace math
{
/**
 * @brief Single segment of a Catmull-Rom spline
 *
 * @tparam T The glm vector type to use for the spline (e.g. glm::vec2, glm::vec3, etc.)
 * @ingroup Math
 */
template<typename T>
class CatmullRomSegment {
public:
    using value_type = typename T::value_type;

    /**
     * @brief Creates a Catmull-Rom segment with all coefficients set to zero
     */
    CatmullRomSegment()
    : ma(value_type(0))
    , mb(value_type(0))
    , mc(value_type(0))
    , md(value_type(0)) {}

    /**
     * @brief Creates a Catmull-Rom segment with the given control points
     *
     * @param p0 The first control point
     * @param p1 The second control point
     * @param p2 The third control point
     * @param p3 The fourth control point
     */
    CatmullRomSegment(const T& p0, const T& p1, const T& p2, const T& p3) { init(p0, p1, p2, p3); }

    /**
     * @brief Creates a Catmull-Rom segment with the given control points
     *
     * @param p0 The first control point
     * @param p1 The second control point
     * @param p2 The third control point
     * @param p3 The fourth control point
     */
    void init(const T& p0, const T& p1, const T& p2, const T& p3) {
        ma = value_type(-0.5) * p0 + value_type(1.5) * p1 + value_type(-1.5) * p2 +
             value_type(0.5) * p3;
        mb =
            value_type(1) * p0 + value_type(-2.5) * p1 + value_type(2) * p2 + value_type(-0.5) * p3;
        mc = value_type(-0.5) * p0 + value_type(0.5) * p2;
        md = p1;
    }

    /**
     * @brief Evaluates the spline at the given parameter t in [0, 1]
     *
     * @param t The interpolation parameter in [0, 1]
     * @return The value of the spline at t
     */
    T evaluate(float t) const { return ((ma * t + mb) * t + mc) * t + md; }

    /**
     * @brief Evaluates the derivative of the spline at the given parameter t in [0, 1]
     *
     * @param t The interpolation parameter in [0, 1]
     * @return The derivative of the spline at t
     */
    T derivative(float t) const { return t * (value_type(3 * t) * ma + value_type(2) * mb) + mc; }

private:
    T ma;
    T mb;
    T mc;
    T md;
};

} // namespace math
} // namespace bl

#endif
