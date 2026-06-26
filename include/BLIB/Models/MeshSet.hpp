#ifndef BLIB_MODELS_MESHSET_HPP
#define BLIB_MODELS_MESHSET_HPP

#include <BLIB/Models/Mesh.hpp>
#include <BLIB/Reflection/ReflectedObject.hpp>
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

    /**
     * @brief Adds a new empty mesh to the set and returns a reference to it
     */
    Mesh& addMesh();

private:
    std::vector<Mesh> meshes;

    friend struct refl::ReflectedObject<MeshSet>;
};

} // namespace mdl

namespace refl
{
template<>
struct ReflectedObject<mdl::MeshSet> {
    inline static const auto spec = makeSpec<mdl::MeshSet>(
        "MeshSet", memberList(defineMember(1, "meshes", &mdl::MeshSet::meshes)));
};
} // namespace refl

} // namespace bl

#endif
