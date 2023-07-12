#ifndef BLIB_MEDIA_GRAPHICS_VERTEXBUFFER_HPP
#define BLIB_MEDIA_GRAPHICS_VERTEXBUFFER_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>

namespace bl
{
namespace rc
{
/**
 * @brief Helper class around SFML's vertex buffer system. This class leverages vertex buffers, if
 *       available, to accelerate rendering time but falls back on plain vertex arrays if buffers
 *       are not available
 *
 * @ingroup Media
 *
 */
class VertexBuffer : public sf::Drawable {
public:
    /**
     * @brief Construct a new Vertex Buffer
     *
     * @param primitives The primitive type to interpret all vertices as
     * @param usage Usage type to hint to the vertex buffer system
     * @param size The number of vertices to create
     */
    VertexBuffer(sf::PrimitiveType primitives, sf::VertexBuffer::Usage usage, unsigned int size);

    /**
     * @brief Sets the number of vertices in the buffer
     *
     * @param size The number of vertices to create
     */
    void resize(unsigned int size);

    /**
     * @brief Returns the number of vertices in the buffer
     *
     */
    unsigned int size() const;

    /**
     * @brief Returns a reference to the vertex at the given index. Note that after making changes
     *        in memory, update() must be called to update the vertex buffer
     *
     * @param i The index to retrieve
     * @return sf::Vertex& Reference to the vertex at the given index
     */
    sf::Vertex& operator[](unsigned int i);

    /**
     * @brief Returns a reference to the vertex at the given index. Note that after making changes
     *        in memory, update() must be called to update the vertex buffer
     *
     * @param i The index to retrieve
     * @return sf::Vertex& Reference to the vertex at the given index
     */
    const sf::Vertex& operator[](unsigned int i) const;

    /**
     * @brief Sends the range of vertices to video memory if vertex buffers are available
     *
     * @param index The start index to send
     * @param count The number of vertices to send
     */
    void update(unsigned int index, unsigned int count);

    /**
     * @brief Sends all vertices to video memory
     *
     */
    void update();

private:
    unsigned int count;
    sf::VertexArray array;
    sf::VertexBuffer buffer;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

} // namespace rc
} // namespace bl

#endif
