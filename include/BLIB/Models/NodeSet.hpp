#ifndef BLIB_MODELS_NODESET_HPP
#define BLIB_MODELS_NODESET_HPP

#include <BLIB/Models/Node.hpp>
#include <BLIB/Reflection/ReflectedObject.hpp>
#include <BLIB/Serialization.hpp>
#include <vector>

namespace bl
{
namespace mdl
{
/**
 * @brief Helper to provide linear storage for model nodes
 *
 * @ingroup Models
 */
class NodeSet {
public:
    /**
     * @brief Constructs a new NodeSet
     */
    NodeSet() = default;

    /**
     * @brief Traverses the scene to reserve space for all nodes
     *
     * @param scene The scene that will be loaded
     */
    void reserveSpace(const aiScene* scene);

    /**
     * @brief Returns the number of nodes in the set
     */
    std::size_t size() const { return nodes.size(); }

    /**
     * @brief Returns the node at the given index
     *
     * @param index The index of the node to return
     */
    Node& getNode(std::size_t index) { return nodes[index]; }

    /**
     * @brief Returns the node at the given index
     *
     * @param index The index of the node to return
     */
    const Node& getNode(std::size_t index) const { return nodes[index]; }

    /**
     * @brief Returns the node with the given name
     *
     * @param name The name of the node to find
     * @return The node with the given name, or nullptr if not found
     */
    const Node* getNode(std::string_view name) const;

    /**
     * @brief Adds a node to the set
     *
     * @param node The node to add
     */
    Node& addNode(std::uint32_t parent);

    /**
     * @brief Used by mergeChildren to remove all nodes except the root
     */
    void clearAllButRoot();

private:
    std::vector<Node> nodes;

    friend struct serial::SerializableObject<NodeSet>;
    friend struct refl::ReflectedObject<NodeSet>;
};

} // namespace mdl

namespace serial
{
template<>
struct SerializableObject<mdl::NodeSet> : public SerializableObjectBase {
    SerializableField<1, mdl::NodeSet, std::vector<mdl::Node>> nodes;

    SerializableObject()
    : SerializableObjectBase("NodeSet")
    , nodes("nodes", *this, &mdl::NodeSet::nodes, SerializableFieldBase::Required{}) {}
};
} // namespace serial

namespace refl
{
template<>
struct ReflectedObject<mdl::NodeSet> {
    inline static const auto spec = makeSpec<mdl::NodeSet>(
        "NodeSet", memberList(defineMember(1, "nodes", &mdl::NodeSet::nodes)));
};
} // namespace refl

} // namespace bl

#endif
