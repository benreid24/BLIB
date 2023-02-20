#ifndef BLIB_RENDER_PRIMITIVES_VERTEX_HPP
#define BLIB_RENDER_PRIMITIVES_VERTEX_HPP

#include <array>
#include <glad/vulkan.h>
#include <glm/glm.hpp>
#include <initializer_list>

namespace bl
{
namespace render
{
/**
 * @brief Base Vertex primitive for the renderer
 *
 * @ingroup Renderer
 *
 */
struct Vertex {
    glm::vec3 pos;
    glm::vec4 color;
    glm::vec2 texCoord;

    /**
     * @brief Creates a white vertex
     */
    Vertex();

    /**
     * @brief Create a vertex from just a position
     *
     * @param position The position of the vertex
     */
    Vertex(const glm::vec3& position);

    /**
     * @brief Create a vertex from a position and a color
     *
     * @param position The position of the vertex
     * @param color The color of the vertex
     */
    Vertex(const glm::vec3& position, const glm::vec4& color);

    /**
     * @brief Create a vertex from a position and texture coordinate
     *
     * @param position The position of the vertex
     * @param texCoord The texture coordinate of the vertex
     */
    Vertex(const glm::vec3& position, const glm::vec2& texCoord);

    /**
     * @brief Create a vertex from each of it's attributes
     *
     * @param position The position of the vertex
     * @param color The color of the vertex
     * @param texCoord The texture coordinate of the vertex
     */
    Vertex(const glm::vec3& position, const glm::vec4& color, const glm::vec2& texCoord);

    /**
     * @brief Creates a vertex from a set of float values. Must be 3, 5, 7, or 9 floats to init the
     *        various permutations of vertex properties
     *
     * @param values The values to init from. Properties in order: pos, color, texCoord
     */
    Vertex(std::initializer_list<float> values);

    /**
     * @brief Returns the vertex binding description for the renderer
     *
     * @return VkVertexInputBindingDescription The vertex binding description
     */
    static VkVertexInputBindingDescription bindingDescription();

    /**
     * @brief Returns the attribute descriptions for each vertex attribute
     *
     * @return std::array<VkVertexInputAttributeDescription, 3> Descriptions for each attribute
     */
    static std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions();
};

} // namespace render
} // namespace bl

#endif
