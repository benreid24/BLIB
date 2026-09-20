#ifndef BLIB_GRAPHICS_INDEXBUFFER3D_HPP
#define BLIB_GRAPHICS_INDEXBUFFER3D_HPP

#include <BLIB/Components/IndexBuffer3D.hpp>
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
class IndexBuffer3D
: public Drawable<com::IndexBuffer3D>
, public bcom::Transform3D
, public bcom::Outline3D {
public:
    /**
     * @brief Creates an uninitialized vertex buffer
     */
    IndexBuffer3D() = default;

    /**
     * @brief Creates the vertex buffer
     *
     * @param world The world to create the object in
     * @param vertexCount The number of vertices to create
     * @param indexCount The number of indices to create
     */
    void create(engine::World& world, unsigned int vertexCount, unsigned int indexCount);

    /**
     * @brief Resizes the vertex buffer to the given size
     *
     * @param vertexCount The number of vertices to resize to
     * @param indexCount The number of indices to resize to
     */
    void resize(unsigned int vertexCount, unsigned int indexCount) {
        component().resize(vertexCount, indexCount);
    }

    /**
     * @brief Returns the number of vertices in the buffer
     */
    std::size_t getVertexCount() const { return component().getVertexCount(); }

    /**
     * @brief Returns the number of indices in the buffer
     */
    std::size_t getIndexCount() const { return component().getIndexCount(); }

    /**
     * @brief Access the vertex at the given index
     *
     * @param i The index to access
     * @return The vertex at the index
     */
    rc::prim::Vertex3D& getVertex(unsigned int i) { return component().getVertex(i); }

    /**
     * @brief Access the index at the given index
     *
     * @param i The index to access
     * @return The index at the index
     */
    std::uint32_t& getIndex(unsigned int i) { return component().getIndex(i); }

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
