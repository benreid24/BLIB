#ifndef BLIB_RENDER_PRIMITIVES_VERTEX3DSKINNED_HPP
#define BLIB_RENDER_PRIMITIVES_VERTEX3DSKINNED_HPP

#include <BLIB/Vulkan.hpp>
#include <array>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace prim
{
/**
 * @brief Vertex type used in 3d pipelines with skinning
 *
 * @ingroup Renderer
 */
struct Vertex3DSkinned {
    glm::vec3 pos;
    glm::vec4 color;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec3 normal;
    std::uint32_t boneIndices[4];
    float boneWeights[4];

    /**
     * @brief Creates a white vertex at the origin
     */
    Vertex3DSkinned();

    /**
     * @brief Creates a vertex with the given position and texture coordinate
     *
     * @param pos The position of the vertex
     * @param texCoord The texture coordinate of the vertex
     */
    Vertex3DSkinned(const glm::vec3& pos, const glm::vec2& texCoord = {});

    /**
     * @brief Creates a vertex with the given position and color
     *
     * @param pos The position of the vertex
     * @param color The color of the vertex
     */
    Vertex3DSkinned(const glm::vec3& pos, const glm::vec4& color);

    /**
     * @brief Returns the vertex binding description for the renderer
     *
     * @return VkVertexInputBindingDescription The vertex binding description
     */
    static VkVertexInputBindingDescription bindingDescription();

    /**
     * @brief Returns the attribute descriptions for each vertex attribute
     *
     * @return Descriptions for each attribute
     */
    static std::array<VkVertexInputAttributeDescription, 8> attributeDescriptions();
};

} // namespace prim
} // namespace rc
} // namespace bl

#endif
