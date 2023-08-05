#ifndef BLIB_RENDER_PRIMTIIVES_SLIDESHOWVERTEX_HPP
#define BLIB_RENDER_PRIMTIIVES_SLIDESHOWVERTEX_HPP

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
 * @brief Vertex type used by slideshows
 *
 * @ingroup Renderer
 */
struct SlideshowVertex {
    glm::vec3 pos;
    std::uint32_t slideshowIndex; // TODO - place at resource level instead of in descriptor set

    /**
     * @brief Creates a 0-initialized vertex
     */
    SlideshowVertex();

    /**
     * @brief Creates a vertex from the given position
     *
     * @param pos The position of the vertex
     */
    SlideshowVertex(const glm::vec3& pos);

    /**
     * @brief Creates a vertex from the given position and slideshow index
     *
     * @param pos The vertex position
     * @param slideshowIndex The slideshow index
     */
    SlideshowVertex(const glm::vec3& pos, std::uint32_t slideshowIndex);

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
    static std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions();
};

} // namespace prim
} // namespace rc
} // namespace bl

#endif
