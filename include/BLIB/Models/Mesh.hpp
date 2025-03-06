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
     * @brief Populates the mesh from the given assimp mesh
     *
     * @param mesh The assimp source mesh
     * @param bones The set of bones in the model
     */
    void populate(const aiMesh* mesh, BoneSet& bones);

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

private:
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;
    unsigned int materialIndex;
};

} // namespace mdl
} // namespace bl

#endif
