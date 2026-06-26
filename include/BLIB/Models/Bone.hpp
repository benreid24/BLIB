#ifndef BLIB_MODELS_BONE_HPP
#define BLIB_MODELS_BONE_HPP

#include <BLIB/Models/ConversionHelpers.hpp>
#include <BLIB/Reflection/ReflectedObject.hpp>
#include <BLIB/Serialization.hpp>
#include <assimp/mesh.h>

namespace bl
{
namespace mdl
{
/**
 * @brief Basic struct representing a bone in a model
 *
 * @ingroup Models
 */
struct Bone {
    std::string name;
    glm::mat4 transform;

    /**
     * @brief Creates a default bone
     */
    Bone()
    : name()
    , transform(1.f) {}

    /**
     * @brief Creates the bone from the given assimp bone
     *
     * @param bone The bone to create from
     */
    Bone(const aiBone* bone)
    : name(bone->mName.data, bone->mName.length)
    , transform(Convert::toMat4(bone->mOffsetMatrix)) {}
};

} // namespace mdl

namespace serial
{
template<>
struct SerializableObject<mdl::Bone> : public SerializableObjectBase {
    SerializableField<1, mdl::Bone, std::string> name;
    SerializableField<2, mdl::Bone, glm::mat4> transform;

    SerializableObject()
    : SerializableObjectBase("Bone")
    , name("name", *this, &mdl::Bone::name, SerializableFieldBase::Required{})
    , transform("transform", *this, &mdl::Bone::transform, SerializableFieldBase::Required{}) {}
};
} // namespace serial

namespace refl
{
template<>
struct ReflectedObject<mdl::Bone> {
    inline static const auto spec = makeSpec<mdl::Bone>(
        "Bone", memberList(defineMember(1, "name", &mdl::Bone::name),
                           defineMember(2, "transform", &mdl::Bone::transform)));
};
} // namespace refl

} // namespace bl

#endif
