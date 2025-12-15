#ifndef BLIB_MODELS_MODEL_HPP
#define BLIB_MODELS_MODEL_HPP

#include <BLIB/Models/BoneSet.hpp>
#include <BLIB/Models/MaterialSet.hpp>
#include <BLIB/Models/NodeSet.hpp>

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
     * @param path The path the model was loaded from
     */
    void populate(const aiScene* scene, const std::string& path = "");

    /**
     * @brief Merges all child nodes up into the parent node and combines meshes that use the same
     *        material. Applies node transform to vertices when merging up
     */
    void mergeChildren();

    /**
     * @brief Returns the root node of the model. Only call after loading
     */
    const Node& getRoot() const { return nodes.getNode(0); }

    /**
     * @brief Returns the set of nodes in the model
     */
    const NodeSet& getNodes() const { return nodes; }

    /**
     * @brief Returns the materials in the model
     */
    const MaterialSet& getMaterials() const { return materials; }

    /**
     * @brief Returns the bones of the model
     */
    const BoneSet& getBones() const { return bones; }

private:
    NodeSet nodes;
    MaterialSet materials;
    BoneSet bones;
};

} // namespace mdl
} // namespace bl

#endif
