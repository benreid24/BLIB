#ifndef BLIB_MODELS_MESH_HPP
#define BLIB_MODELS_MESH_HPP

#include <BLIB/Models/Vertex.hpp>
#include <assimp/mesh.h>
#include <cstdint>
#include <vector>

namespace bl
{
namespace mdl
{
class BoneSet;

/**
 * @brief Basic class containing data for a single mesh in a model
 *
 * @ingroupo Models
 */
class Mesh {
public:
    /**
     * @brief Creates an empty mesh
     */
    Mesh();

    /**
     * @brief Basic copy constructor
     */
    Mesh(const Mesh&) = default;

    /**
     * @brief Creates the mesh from the source mesh but applies the given transform to all vertices
     *
     * @param src The source mesh
     * @param transform The transform to apply to all vertices
     */
    Mesh(const Mesh& src, const glm::mat4& transform);

    /**
     * @brief Populates the mesh from the given assimp mesh
     *
     * @param mesh The assimp source mesh
     * @param bones The set of bones in the model
     */
    void populate(const aiMesh* mesh, BoneSet& bones);

    /**
     * @brief Merges the vertices and indices of the given mesh into this mesh
     *
     * @param other The mesh to merge into this one
     * @param transform The transform to apply to the vertices of the other mesh
     */
    void combine(const Mesh& other, const glm::mat4& transform);

    /**
     * @brief Returns the vertices of the mesh
     */
    const std::vector<Vertex>& getVertices() const { return vertices; }

    /**
     * @brief Returns the indices of the mesh
     */
    const std::vector<std::uint32_t>& getIndices() const { return indices; }

    /**
     * @brief Returns the material index of the mesh
     */
    unsigned int getMaterialIndex() const { return materialIndex; }

    /**
     * @brief Flips the V texture coordinates of all vertices in the mesh
     */
    void flipUVs();

    /**
     * @brief Returns whether the mesh is skinned
     */
    bool getIsSkinned() const { return isSkinned; }

private:
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;
    unsigned int materialIndex;
    bool isSkinned;

    void transformVertices(const std::vector<Vertex>& src, const glm::mat4& transform);
};

} // namespace mdl
} // namespace bl

#endif
