#ifndef BLIB_OBJECTS_3D_VERTEXBUFFER_HPP
#define BLIB_OBJECTS_3D_VERTEXBUFFER_HPP

#include <BLIB/Components/VertexBuffer3D.hpp>
#include <BLIB/Objects/3D/DrawableObject3D.hpp>

namespace bl
{
namespace obj
{
namespace o3d
{
/**
 * @brief Graphics object for raw vertex buffers
 *
 * @ingroup Objects
 */
class VertexBuffer : public DrawableObject3D<com::VertexBuffer3D> {
public:
    /**
     * @brief Creates an uninitialized vertex buffer
     */
    VertexBuffer() = default;

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
};
} // namespace o3d
} // namespace obj
} // namespace bl

#endif
