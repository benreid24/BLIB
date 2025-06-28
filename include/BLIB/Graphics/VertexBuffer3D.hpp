#ifndef BLIB_GRAPHICS_VERTEXBUFFER3D_HPP
#define BLIB_GRAPHICS_VERTEXBUFFER3D_HPP

#include <BLIB/Components/VertexBuffer3D.hpp>
#include <BLIB/Graphics/Components/Outline3D.hpp>
#include <BLIB/Graphics/Components/Textured.hpp>
#include <BLIB/Graphics/Components/Transform3D.hpp>
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
class VertexBuffer3D
: public Drawable<com::VertexBuffer3D>
, public bcom::Transform3D
, public bcom::Outline3D {
public:
    /**
     * @brief Creates an uninitialized vertex buffer
     */
    VertexBuffer3D() = default;

    /**
     * @brief Creates the vertex buffer
     *
     * @param world The world to create the object in
     * @param vertexCount The number of vertices to create
     */
    void create(engine::World& world, unsigned int vertexCount);

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
    rc::prim::Vertex3D& operator[](unsigned int i) { return component()[i]; }

    /**
     * @brief Access the vertex at the given index
     *
     * @param i The index to access
     * @return The vertex at the index
     */
    const rc::prim::Vertex3D& operator[](unsigned int i) const { return component()[i]; }

    /**
     * @brief Writes the buffer and syncs the draw parameters
     */
    void commit();

private:
    virtual void scaleToSize(const glm::vec2& size) override;
};

} // namespace gfx
} // namespace bl

#endif
