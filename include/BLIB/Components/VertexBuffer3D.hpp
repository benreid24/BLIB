#ifndef BLIB_COMPONENTS_VERTEXBUFFER3D_HPP
#define BLIB_COMPONENTS_VERTEXBUFFER3D_HPP

#include <BLIB/Render/Buffers/VertexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Config/MaterialPipelineIds.hpp>

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
class VertexBuffer3D : public rc::rcom::DrawableBase {
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
     * @brief Resizes the vertex buffer to the given size
     *
     * @param vertexCount The number of vertices to resize to
     * @param copyOld Whether to copy the vertices to the new buffer if grown
     */
    void resize(unsigned int vertexCount, bool copyOld = true);

    /**
     * @brief Access the vertex at the given index
     *
     * @param i The index to access
     * @return The vertex at the index
     */
    rc::prim::Vertex3D& operator[](unsigned int i) { return buffer.vertices()[i]; }

    /**
     * @brief Access the vertex at the given index
     *
     * @param i The index to access
     * @return The vertex at the index
     */
    const rc::prim::Vertex3D& operator[](unsigned int i) const { return buffer.vertices()[i]; }

    /**
     * @brief Writes the buffer and syncs the draw parameters
     */
    void commit();

    /**
     * @brief Returns the default material pipeline for rendering
     */
    virtual std::uint32_t getDefaultMaterialPipelineId() const override {
        return rc::cfg::MaterialPipelineIds::Mesh3D;
    }

private:
    rc::buf::VertexBuffer3D buffer;
};

} // namespace com
} // namespace bl

#endif
