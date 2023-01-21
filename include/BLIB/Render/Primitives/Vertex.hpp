#ifndef BLIB_RENDER_PRIMITIVES_VERTEX_HPP
#define BLIB_RENDER_PRIMITIVES_VERTEX_HPP

#include <array>
#include <glad/vulkan.h>
#include <glm/glm.hpp>

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
    glm::vec3 color;
    glm::vec2 texCoord;

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
