#ifndef BLIB_MODELS_MATERIALSET_HPP
#define BLIB_MODELS_MATERIALSET_HPP

#include <BLIB/Models/Material.hpp>
#include <assimp/material.h>
#include <assimp/scene.h>
#include <vector>

namespace bl
{
namespace mdl
{
/**
 * @brief Contains the set of materials in a model
 *
 * @ingroup Models
 */
class MaterialSet {
public:
    /**
     * @brief Creates an empty material set
     */
    MaterialSet() = default;

    /**
     * @brief Populates the material set from the given model
     *
     * @param scene The assimp scene to load from
     * @param modelPath The path the model was loaded from
     */
    void populate(const aiScene* scene, const std::string& modelPath);

    /**
     * @brief Returns the material with the given index
     *
     * @param i The index of the materials to fetch
     * @return The material at the given index
     */
    const Material& getMaterial(unsigned int i) const { return materials[i]; }

    /**
     * @brief Returns the number of materials in the set
     */
    unsigned int numMaterials() const { return materials.size(); }

private:
    std::vector<Material> materials;
};

} // namespace mdl
} // namespace bl

#endif
