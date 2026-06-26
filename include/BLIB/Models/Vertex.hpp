#ifndef BLIB_MODELS_VERTEX_HPP
#define BLIB_MODELS_VERTEX_HPP

#include <BLIB/Reflection/ReflectedObject.hpp>
#include <BLIB/Serialization.hpp>
#include <array>
#include <glm/glm.hpp>

namespace bl
{
/// Contains data structures and functionality for 3d models and animations
namespace mdl
{
struct Bone;

/**
 * @brief Basic model vertex
 *
 * @ingroup Models
 */
struct Vertex {
    glm::vec3 pos;
    glm::vec4 color;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec3 normal;
    std::array<std::int32_t, 4> boneIds;
    std::array<float, 4> boneWeights;

    /**
     * @brief Initializes the vertex to sane defaults
     */
    Vertex()
    : pos()
    , color(1.f)
    , texCoord()
    , tangent()
    , bitangent()
    , normal()
    , boneIds{-1, -1, -1, -1}
    , boneWeights{} {}
};

} // namespace mdl

namespace serial
{
template<>
struct SerializableObject<mdl::Vertex> : public SerializableObjectBase {
    SerializableField<1, mdl::Vertex, glm::vec3> pos;
    SerializableField<2, mdl::Vertex, glm::vec4> color;
    SerializableField<3, mdl::Vertex, glm::vec2> texCoord;
    SerializableField<4, mdl::Vertex, glm::vec3> tangent;
    SerializableField<5, mdl::Vertex, glm::vec3> bitangent;
    SerializableField<6, mdl::Vertex, glm::vec3> normal;
    SerializableField<7, mdl::Vertex, std::array<std::int32_t, 4>> boneIds;
    SerializableField<8, mdl::Vertex, std::array<float, 4>> boneWeights;

    SerializableObject()
    : SerializableObjectBase("Vertex")
    , pos("pos", *this, &mdl::Vertex::pos, SerializableFieldBase::Required{})
    , color("color", *this, &mdl::Vertex::color, SerializableFieldBase::Required{})
    , texCoord("texCoord", *this, &mdl::Vertex::texCoord, SerializableFieldBase::Required{})
    , tangent("tangent", *this, &mdl::Vertex::tangent, SerializableFieldBase::Required{})
    , bitangent("bitangent", *this, &mdl::Vertex::bitangent, SerializableFieldBase::Required{})
    , normal("normal", *this, &mdl::Vertex::normal, SerializableFieldBase::Required{})
    , boneIds("boneIds", *this, &mdl::Vertex::boneIds, SerializableFieldBase::Required{})
    , boneWeights("boneWeights", *this, &mdl::Vertex::boneWeights,
                  SerializableFieldBase::Required{}) {}
};
} // namespace serial

namespace refl
{
template<>
struct ReflectedObject<mdl::Vertex> {
    inline static const auto spec = makeSpec<mdl::Vertex>(
        "Vertex", memberList(defineMember(1, "pos", &mdl::Vertex::pos),
                             defineMember(2, "color", &mdl::Vertex::color),
                             defineMember(3, "texCoord", &mdl::Vertex::texCoord),
                             defineMember(4, "tangent", &mdl::Vertex::tangent),
                             defineMember(5, "bitangent", &mdl::Vertex::bitangent),
                             defineMember(6, "normal", &mdl::Vertex::normal),
                             defineMember(7, "boneIds", &mdl::Vertex::boneIds),
                             defineMember(8, "boneWeights", &mdl::Vertex::boneWeights)));
};
} // namespace refl

} // namespace bl

#endif
