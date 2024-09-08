#ifndef BLIB_GRAPHICS_VERTEXBUFFER2D
#define BLIB_GRAPHICS_VERTEXBUFFER2D

#include <BLIB/Components/VertexBuffer.hpp>
#include <BLIB/Graphics/Components/Textured.hpp>
#include <BLIB/Graphics/Components/Transform2D.hpp>
#include <BLIB/Graphics/Drawable.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Graphics object for raw vertex buffers
 *
 * @ingroup Graphics
 */
class VertexBuffer2D
: public Drawable<com::VertexBuffer>
, public bcom::Transform2D
, public bcom::Textured {
public:
    /**
     * @brief Creates an uninitialized vertex buffer
     */
    VertexBuffer2D() = default;

    /**
     * @brief Creates the vertex buffer
     *
     * @param world The world to create the object in
     * @param vertexCount The number of vertices to create
     */
    void create(engine::World& world, unsigned int vertexCount);

    /**
     * @brief Creates the vertex buffer with a texture
     *
     * @param world The world to create the object in
     * @param vertexCount The number of vertices to create
     * @param texture The texture to apply
     */
    void create(engine::World& world, unsigned int vertexCount, rc::res::TextureRef texture);

    /**
     * @brief Resizes the vertex buffer to the given size
     *
     * @param vertexCount The number of vertices to resize to
     * @param copyOld Whether to copy the vertices to the new buffer if grown
     */
    void resize(unsigned int vertexCount, bool copyOld = true) {
        component().resize(vertexCount, copyOld);
    }

    /**
     * @brief Returns the number of vertices in the buffer
     */
    std::size_t getSize() const { return component().getSize(); }

    /**
     * @brief Access the vertex at the given index
     *
     * @param i The index to access
     * @return The vertex at the index
     */
    rc::prim::Vertex& operator[](unsigned int i) { return component()[i]; }

    /**
     * @brief Access the vertex at the given index
     *
     * @param i The index to access
     * @return The vertex at the index
     */
    const rc::prim::Vertex& operator[](unsigned int i) const { return component()[i]; }

    /**
     * @brief Writes the buffer and syncs the draw parameters
     */
    void commit() { component().commit(); }

    /**
     * @brief Helper method to set the scale so that the entity is a certain size
     *
     * @param size The size to scale to
     */
    virtual void scaleToSize(const glm::vec2& size) override;
};

} // namespace gfx
} // namespace bl

#endif
