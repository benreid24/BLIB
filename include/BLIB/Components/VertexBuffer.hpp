#ifndef BLIB_COMPONENTS_VERTEXBUFFER_HPP
#define BLIB_COMPONENTS_VERTEXBUFFER_HPP

#include <BLIB/Render/Buffers/VertexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

namespace com
{
/**
 * @brief Renderable component for a raw vertex buffer
 *
 * @ingroup Components
 */
class VertexBuffer : public rc::rcom::DrawableBase {
public:
    /**
     * @brief Creates the vertex buffer
     *
     * @param engine The game engine instance
     * @param vertexCount The number of vertices to create
     */
    void create(engine::Engine& engine, unsigned int vertexCount);

    /**
     * @brief Returns the number of vertices in the buffer
     */
    std::size_t getSize() const { return buffer.vertexCount(); }

    /**
     * @brief Access the vertex at the given index
     *
     * @param i The index to access
     * @return The vertex at the index
     */
    rc::prim::Vertex& operator[](unsigned int i) { return buffer.vertices()[i]; }

    /**
     * @brief Access the vertex at the given index
     *
     * @param i The index to access
     * @return The vertex at the index
     */
    const rc::prim::Vertex& operator[](unsigned int i) const { return buffer.vertices()[i]; }

    /**
     * @brief Writes the buffer and syncs the draw parameters
     */
    void commit();

private:
    rc::buf::VertexBuffer buffer;
};

} // namespace com
} // namespace bl

#endif
