#ifndef BLIB_MODELS_NODE_HPP
#define BLIB_MODELS_NODE_HPP

#include <BLIB/Reflection/ReflectedObject.hpp>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <optional>
#include <vector>

namespace bl
{
namespace mdl
{
class NodeSet;
class MeshSet;
class BoneSet;

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
     * @brief Returns the children of this node
     */
    const std::vector<std::uint32_t>& getChildren() const { return children; }

    /**
     * @brief Returns the meshes of this node
     */
    const std::vector<unsigned int>& getMeshes() const { return meshes; }

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

    /**
     * @brief Returns the name of this node
     */
    const std::string& getName() const { return name; }

    /**
     * @brief Initializes the node with the given data
     *
     * @param name The name of the node
     * @param transform The local transform of the node
     * @param meshIndices The indices of the meshes attached to this node
     * @param boneIndex The index of the bone for this node, if any
     */
    void init(const std::string& name, const glm::mat4& transform,
              const std::vector<std::uint32_t>& meshIndices = {},
              std::optional<std::uint32_t> boneIndex        = std::nullopt);

private:
    std::uint32_t parent;
    std::uint32_t ownIndex;
    std::string name;
    std::vector<std::uint32_t> children;
    std::vector<std::uint32_t> meshes;
    glm::mat4 transform;
    std::optional<std::uint32_t> boneIndex;

    friend struct refl::ReflectedObject<Node>;
};

} // namespace mdl

namespace refl
{
template<>
struct ReflectedObject<mdl::Node> {
    inline static const auto spec = makeSpec<mdl::Node>(
        "Node", memberList(defineMember(1, "parent", &mdl::Node::parent),
                           defineMember(2, "ownIndex", &mdl::Node::ownIndex),
                           defineMember(3, "name", &mdl::Node::name),
                           defineMember(4, "children", &mdl::Node::children),
                           defineMember(5, "meshes", &mdl::Node::meshes),
                           defineMember(6, "transform", &mdl::Node::transform),
                           defineMember(7, "boneIndex", &mdl::Node::boneIndex)));
};
} // namespace refl

} // namespace bl

#endif
