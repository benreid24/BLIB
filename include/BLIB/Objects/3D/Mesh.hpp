#ifndef BLIB_OBJECTS_3D_MESH_HPP
#define BLIB_OBJECTS_3D_MESH_HPP

#include <BLIB/Components/Mesh.hpp>
#include <BLIB/Objects/Components/Outline3D.hpp>
#include <BLIB/Objects/Components/Textured.hpp>
#include <BLIB/Objects/Components/Transform3D.hpp>
#include <BLIB/Objects/Drawable.hpp>

namespace bl
{
namespace obj
{
namespace o3d
{
/**
 * @brief Basic mesh object
 *
 * @ingroup Objects
 */
template<typename TVertex, std::uint32_t DefaultMaterialPipelineId>
class Mesh
: public Drawable<com::Mesh<TVertex, DefaultMaterialPipelineId>>
, public bcom::Transform3D
, public bcom::Outline3D {
public:
    /**
     * @brief Creates an uninitialized mesh
     */
    Mesh() = default;

    /**
     * @brief Creates the mesh
     *
     * @param world The world to create the object in
     * @param vertexCount The number of vertices to create
     * @param indexCount The number of indices to create
     */
    void create(engine::World& world, unsigned int vertexCount, unsigned int indexCount);

    /**
     * @brief Resizes the mesh to the given size
     *
     * @param vertexCount The number of vertices to resize to
     * @param indexCount The number of indices to resize to
     */
    void resize(unsigned int vertexCount, unsigned int indexCount) {
        this->component().resize(vertexCount, indexCount);
    }

    /**
     * @brief Returns the number of vertices in the buffer
     */
    std::size_t getVertexCount() const { return this->component().getVertexCount(); }

    /**
     * @brief Returns the number of indices in the buffer
     */
    std::size_t getIndexCount() const { return this->component().getIndexCount(); }

    /**
     * @brief Access the vertex at the given index
     *
     * @param i The index to access
     * @return The vertex at the index
     */
    rc::prim::Vertex3D& getVertex(unsigned int i) {
        return this->component().gpuBuffer.vertices()[i];
    }

    /**
     * @brief Access the index at the given index
     *
     * @param i The index to access
     * @return The index at the index
     */
    std::uint32_t& getIndex(unsigned int i) { return this->component().gpuBuffer.indices()[i]; }

    /**
     * @brief Writes the buffer and syncs the draw parameters
     */
    void commit();

private:
    virtual void scaleToSize(const glm::vec2&) override {}
};

/**
 * @brief A basic 3d mesh object
 *
 * @ingroup Objects
 */
using BasicMesh = Mesh<rc::prim::Vertex3D, rc::cfg::MaterialPipelineIds::Mesh3DMaterial>;

/**
 * @brief A skinned 3d mesh object
 *
 * @ingroup Objects
 */
using SkinnedMesh = Mesh<rc::prim::Vertex3DSkinned, rc::cfg::MaterialPipelineIds::Mesh3DSkinned>;

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TVertex, std::uint32_t DefaultMaterialPipelineId>
void Mesh<TVertex, DefaultMaterialPipelineId>::create(engine::World& world,
                                                      unsigned int vertexCount,
                                                      unsigned int indexCount) {
    Drawable<com::Mesh<TVertex, DefaultMaterialPipelineId>>::create(world);
    Transform3D::create(world.engine().ecs(), this->entity());
    Outline3D::init(world.engine().ecs(), this->entity(), &this->component());
    this->component().create(world.engine().renderer(), vertexCount, indexCount);
}

template<typename TVertex, std::uint32_t DefaultMaterialPipelineId>
void Mesh<TVertex, DefaultMaterialPipelineId>::commit() {
    TVertex::computeTBN(this->component().gpuBuffer.vertices().data(),
                        this->component().gpuBuffer.indices().data(),
                        this->component().gpuBuffer.indices().size());
    this->component().commit();
}

} // namespace o3d
} // namespace obj
} // namespace bl

#endif
