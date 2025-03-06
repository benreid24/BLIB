#ifndef BLIB_MODELS_MODEL_HPP
#define BLIB_MODELS_MODEL_HPP

#include <BLIB/Models/BoneSet.hpp>
#include <BLIB/Models/MaterialSet.hpp>
#include <BLIB/Models/Node.hpp>

namespace bl
{
namespace mdl
{
/**
 * @brief Top level class representing a 3d model
 *
 * @ingroup Models
 */
class Model {
public:
    /**
     * @brief Creates an empty model
     */
    Model() = default;

    /**
     * @brief Populates the model from the Assimp scene
     *
     * @param scene The scene to populate from
     */
    void populate(const aiScene* scene);

    /**
     * @brief Returns the root node of the model
     */
    const Node& getRoot() const { return root; }

    /**
     * @brief Returns the materials in the model
     */
    const MaterialSet& getMaterials() const { return materials; }

    /**
     * @brief Returns the bones of the model
     */
    const BoneSet& getBones() const { return bones; }

private:
    Node root;
    MaterialSet materials;
    BoneSet bones;
};

} // namespace mdl
} // namespace bl

#endif
