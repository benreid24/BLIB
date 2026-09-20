#ifndef BLIB_COMPONENTS_INDEXBUFFER3D_HPP
#define BLIB_COMPONENTS_INDEXBUFFER3D_HPP

#include <BLIB/Render/Buffers/IndexBuffer.hpp>
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
 * @brief Renderable component for a raw index buffer
 *
 * @ingroup Components
 */
class IndexBuffer3D : public rc::rcom::DrawableBase {
public:
    /**
     * @brief Creates the index buffer
     *
     * @param engine The game engine instance
     * @param vertexCount The number of vertices in the buffer
     * @param indexCount The number of indices in the buffer
     */
    void create(engine::Engine& engine, unsigned int vertexCount, unsigned int indexCount);

    /**
     * @brief Returns the number of vertices in the buffer
     */
    std::size_t getVertexCount() const { return buffer.vertexCount(); }

    /**
     * @brief Returns the number of indices in the buffer
     */
    std::size_t getIndexCount() const { return buffer.indexCount(); }

    /**
     * @brief Resizes the index buffer to the given size
     *
     * @param vertexCount The number of vertices to resize to
     * @param indexCount The number of indices to resize to
     */
    void resize(unsigned int vertexCount, unsigned int indexCount);

    /**
     * @brief Access the vertex at the given index
     *
     * @param i The index to access
     * @return The vertex at the index
     */
    rc::prim::Vertex3D& getVertex(unsigned int i) { return buffer.vertices()[i]; }

    /**
     * @brief Access the index at the given index
     *
     * @param i The index to access
     * @return The index at the index
     */
    std::uint32_t& getIndex(unsigned int i) { return buffer.indices()[i]; }

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
    rc::buf::IndexBuffer3D buffer;
};

} // namespace com
} // namespace bl

#endif
