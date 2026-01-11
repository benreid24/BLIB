#ifndef BLIB_MODELS_MESHSET_HPP
#define BLIB_MODELS_MESHSET_HPP

#include <BLIB/Models/Mesh.hpp>
#include <assimp/scene.h>
#include <vector>

namespace bl
{
namespace mdl
{
class BoneSet;

/**
 * @brief Collection of meshes on a model
 *
 * @ingroup Models
 */
class MeshSet {
public:
    /**
     * @brief Creates an empty mesh set
     */
    MeshSet() = default;

    /**
     * @brief Populates the mesh set from the given model
     *
     * @param scene The model to load from
     * @param bones The set of bones in the model
     */
    void populate(const aiScene* scene, BoneSet& bones);

    /**
     * @brief Returns the number of meshes in the set
     */
    unsigned int getMeshCount() const { return meshes.size(); }

    /**
     * @brief Fetches the mesh at the given index
     *
     * @param index The index of the mesh to get
     * @return The mesh at the given index
     */
    const Mesh& getMesh(unsigned int index) const { return meshes[index]; }

    /**
     * @brief Fetches the mesh at the given index
     *
     * @param index The index of the mesh to get
     * @return The mesh at the given index
     */
    Mesh& getMesh(unsigned int index) { return meshes[index]; }

    /**
     * @brief Flips the V texture coordinates of all meshes in the set
     */
    void flipUVs();

private:
    std::vector<Mesh> meshes;
};

} // namespace mdl
} // namespace bl

#endif
