#ifndef BLIB_COMPONENTS_HEIGHTMAP_HPP
#define BLIB_COMPONENTS_HEIGHTMAP_HPP

#include <BLIB/Containers/Vector2d.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace com
{
/**
 * @brief A basic heightmap component
 *
 * @ingroup Components
 */
struct Heightmap {
    ctr::Vector2D<float> heightmap;
    glm::vec2 offset;
    glm::vec2 size;

    /**
     * @brief Creates an empty heightmap
     */
    Heightmap();

    /**
     * @brief Creates a heightmap with the given parameters
     *
     * @param offset The offset of the top left corner of the heightmap in world coordinates
     * @param size The size of the heightmap in world coordinates
     * @param step The distance between each vertex in world coordinates
     */
    void create(const glm::vec2& offset, const glm::vec2& size, float step);

    /**
     * @brief Creates a heightmap with the given parameters
     *
     * @param heightmap The heightmap data to use
     * @param offset The offset of the top left corner of the heightmap in world coordinates
     * @param size The size of the heightmap in world coordinates
     */
    void create(const ctr::Vector2D<float>& heightmap, const glm::vec2& offset,
                const glm::vec2& size);

    /**
     * @brief Creates a heightmap with the given parameters
     *
     * @param heightmap The heightmap data to use
     * @param offset The offset of the top left corner of the heightmap in world coordinates
     * @param size The size of the heightmap in world coordinates
     */
    void create(ctr::Vector2D<float>&& heightmap, const glm::vec2& offset, const glm::vec2& size);

    /**
     * @brief Samples the heightmap at the given position
     *
     * @param position The position in world coordinates to sample the heightmap at
     * @return The height of the heightmap at the given position
     */
    float sampleHeight(const glm::vec2& position) const;
};

} // namespace com
} // namespace bl

#endif
