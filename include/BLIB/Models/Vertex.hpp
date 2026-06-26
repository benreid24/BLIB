#ifndef BLIB_MODELS_VERTEX_HPP
#define BLIB_MODELS_VERTEX_HPP

#include <BLIB/Reflection/ReflectedObject.hpp>
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
