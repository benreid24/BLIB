#ifndef BLIB_MODELS_KEYFRAMEVECTOR_HPP
#define BLIB_MODELS_KEYFRAMEVECTOR_HPP

#include <BLIB/Reflection/ReflectedObject.hpp>
#include <BLIB/Serialization.hpp>
#include <assimp/anim.h>
#include <cstdint>
#include <glm/glm.hpp>

namespace bl
{
namespace mdl
{
/**
 * @brief A keyframe for a vector3 value in an animation
 *
 * @ingroup Models
 */
struct KeyframeVector {
    /**
     * @brief The interpolation type for the keyframe
     */
    enum Interpolation : std::uint8_t {
        Step            = aiAnimInterpolation_Step,
        Linear          = aiAnimInterpolation_Linear,
        SphericalLinear = aiAnimInterpolation_Spherical_Linear,
        CubicSpline     = aiAnimInterpolation_Cubic_Spline
    };

    double time;
    glm::vec3 value;
    Interpolation interpolation;

    /**
     * @brief Creates an empty keyframe
     */
    KeyframeVector();

    /**
     * @brief Populates the keyframe from the given assimp vector key
     *
     * @param src The Assimp keyframe
     */
    void populate(const aiVectorKey& src);
};

} // namespace mdl

namespace serial
{
template<>
struct SerializableObject<mdl::KeyframeVector> : public SerializableObjectBase {
    SerializableField<1, mdl::KeyframeVector, double> time;
    SerializableField<2, mdl::KeyframeVector, glm::vec3> value;
    SerializableField<3, mdl::KeyframeVector, mdl::KeyframeVector::Interpolation> interpolation;

    SerializableObject()
    : SerializableObjectBase("KeyframeVector")
    , time("time", *this, &mdl::KeyframeVector::time, SerializableFieldBase::Required{})
    , value("value", *this, &mdl::KeyframeVector::value, SerializableFieldBase::Required{})
    , interpolation("interpolation", *this, &mdl::KeyframeVector::interpolation,
                    SerializableFieldBase::Required{}) {}
};

} // namespace serial

namespace refl
{
template<>
struct ReflectedObject<mdl::KeyframeVector> {
    inline static const auto spec = makeSpec<mdl::KeyframeVector>(
        "KeyframeVector",
        memberList(defineMember(1, "time", &mdl::KeyframeVector::time),
                   defineMember(2, "value", &mdl::KeyframeVector::value),
                   defineMember(3, "interpolation", &mdl::KeyframeVector::interpolation)));
};
} // namespace refl

} // namespace bl

#endif
