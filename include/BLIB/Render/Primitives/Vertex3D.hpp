#ifndef BLIB_RENDER_PRIMITIVES_VERTEX3D_HPP
#define BLIB_RENDER_PRIMITIVES_VERTEX3D_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Models/Vertex.hpp>
#include <BLIB/Vulkan.hpp>
#include <array>
#include <glm/glm.hpp>
#include <type_traits>

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
     * @brief Computes the TBN matrix for the given triangle
     *
     * @param v1 The first vertex of the triangle
     * @param v2 The second vertex of the triangle
     * @param v3 The third vertex of the triangle
     */
    static void computeSingleTBN(Vertex3D& v1, Vertex3D& v2, Vertex3D& v3);

    /**
     * @brief Computes TBN vectors for the group of vertices (assumed to be triangle list)
     *
     * @tparam TVertex The type of vertex to process
     * @param vertices Pointer to the first vertex
     * @param nVertices The number of vertices to process
     */
    template<typename TVertex>
    static void computeTBN(TVertex* vertices, std::size_t nVertices);

    /**
     * @brief Computes TBN vectors for the group of vertices as an index buffer
     *
     * @param vertices Pointer to the first vertex
     * @param indices Pointer to the first index
     * @param nIndices The number of indices to process
     */
    template<typename TVertex>
    static void computeTBN(TVertex* vertices, std::uint32_t* indices, std::size_t nIndices);

    /**
     * @brief Copies this vertex from a model vertex
     *
     * @param vertex The model vertex to update from
     * @return A reference to this vertex
     */
    Vertex3D& operator=(const mdl::Vertex& vertex);

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

    /**
     * @brief Returns the attribute descriptions for only the positions
     *
     * @return Descriptions for only the positions
     */
    static std::array<VkVertexInputAttributeDescription, 1> attributeDescriptionsPositionsOnly();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TVertex>
void Vertex3D::computeTBN(TVertex* vertices, std::size_t n) {
    static_assert(std::is_base_of_v<Vertex3D, TVertex>,
                  "Vertex type must be derived from Vertex3D");

    if (n % 3 != 0) { BL_LOG_ERROR << "Number of vertices is not divisible by 3"; }

    for (std::size_t i = 0; i < n; i += 3) {
        computeSingleTBN(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
}

template<typename TVertex>
void Vertex3D::computeTBN(TVertex* vertices, std::uint32_t* indices, std::size_t n) {
    static_assert(std::is_base_of_v<Vertex3D, TVertex>,
                  "Vertex type must be derived from Vertex3D");

    if (n % 3 != 0) { BL_LOG_ERROR << "Number of indices is not divisible by 3"; }

    for (std::size_t i = 0; i < n; i += 3) {
        computeSingleTBN(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
    }
}

} // namespace prim
} // namespace rc
} // namespace bl

#endif
