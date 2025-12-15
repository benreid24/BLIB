#ifndef BLIB_MODELS_NODE_HPP
#define BLIB_MODELS_NODE_HPP

#include <BLIB/Models/Mesh.hpp>
#include <assimp/scene.h>
#include <optional>

namespace bl
{
namespace mdl
{
class NodeSet;

/**
 * @brief Represents a node in a model hierarchy
 *
 * @ingroup Models
 */
class Node {
public:
    /**
     * @brief Creates an empty node
     */
    Node();

    /**
     * @brief Creates the node from its parent and own indices
     *
     * @param parentIndex The index of the parent node
     * @param ownIndex The index of this node
     */
    Node(std::uint32_t parentIndex, std::uint32_t ownIndex);

    /**
     * @brief Populates the node from the given assimp node
     *
     * @param nodeSet The storage for new nodes
     * @param scene The top level scene
     * @param src The node to populate from
     * @param bones The set of bones in the model
     */
    void populate(NodeSet& nodeSet, const aiScene* scene, const aiNode* src, BoneSet& bones);

    /**
     * @brief Merges all child node meshes into this mesh and combines meshes that use the same
     *        material. Removes the child nodes
     *
     * @param nodeSet The set of nodes in the model
     */
    void mergeChildren(NodeSet& nodeSet);

    /**
     * @brief Returns the children of this node
     */
    const std::vector<std::uint32_t>& getChildren() const { return children; }

    /**
     * @brief Returns the meshes of this node
     */
    const std::vector<Mesh>& getMeshes() const { return meshes; }

    /**
     * @brief Returns the transform of this node
     */
    const glm::mat4& getTransform() const { return transform; }

    /**
     * @brief Returns the parent of this node
     */
    std::uint32_t getParentIndex() const { return parent; }

    /**
     * @brief Returns the index of this node
     */
    std::uint32_t getOwnIndex() const { return ownIndex; }

    /**
     * @brief Returns the index of the bone for this node, if it has one
     */
    const std::optional<unsigned int>& getBoneIndex() const { return boneIndex; }

private:
    std::uint32_t parent;
    std::uint32_t ownIndex;
    std::string name;
    std::vector<std::uint32_t> children;
    std::vector<Mesh> meshes;
    glm::mat4 transform;
    std::optional<unsigned int> boneIndex;
};

} // namespace mdl
} // namespace bl

#endif
