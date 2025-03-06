#ifndef BLIB_MODELS_VERTEX_HPP
#define BLIB_MODELS_VERTEX_HPP

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
    std::int32_t boneIds[4];
    float boneWeights[4];

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
} // namespace bl

#endif
