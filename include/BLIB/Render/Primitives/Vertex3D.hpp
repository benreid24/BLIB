#ifndef BLIB_RENDER_PRIMITIVES_VERTEX3D_HPP
#define BLIB_RENDER_PRIMITIVES_VERTEX3D_HPP

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
 * @brief Vertex type used in 3d pipelines
 *
 * @ingroup Renderer
 */
struct Vertex3D {
    glm::vec3 pos;
    glm::vec4 color;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    glm::vec3 normal;

    /**
     * @brief Creates a white vertex at the origin
     */
    Vertex3D();

    /**
     * @brief Creates a vertex with the given position and texture coordinate
     *
     * @param pos The position of the vertex
     * @param texCoord The texture coordinate of the vertex
     */
    Vertex3D(const glm::vec3& pos, const glm::vec2& texCoord = {});

    /**
     * @brief Creates a vertex with the given position and color
     *
     * @param pos The position of the vertex
     * @param color The color of the vertex
     */
    Vertex3D(const glm::vec3& pos, const glm::vec4& color);

    /**
     * @brief Computes TBN vectors for the group of vertices (assumed to be triangle list)
     *
     * @param vertices Pointer to the first vertex
     * @param nVertices The number of vertices to process
     */
    static void computeTBN(Vertex3D* vertices, std::size_t nVertices);

    /**
     * @brief Computes TBN vectors for the group of vertices as an index buffer
     *
     * @param vertices Pointer to the first vertex
     * @param indices Pointer to the first index
     * @param nIndices The number of indices to process
     */
    static void computeTBN(Vertex3D* vertices, std::uint32_t* indices, std::size_t nIndices);

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
    static std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions();
};

} // namespace prim
} // namespace rc
} // namespace bl

#endif
