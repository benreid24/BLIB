#ifndef BLIB_PARSER_NODE_HPP
#define BLIB_PARSER_NODE_HPP

#include <memory>
#include <string>
#include <vector>

namespace bl
{
namespace parser
{
/**
 * @brief Represents a basic parse node. Can be both tokens (terminals) or non terminal grammar
 * elements
 * @ingroup Parser
 *
 */
struct Node {
    typedef int Type;
    static constexpr Type None = -1;
    static constexpr Type EOI  = -2;
    typedef std::vector<Type> Sequence;

    typedef std::shared_ptr<Node> Ptr;

    Type type;
    Node* parent;
    std::vector<Ptr> children;
    std::string data;

    int sourceLine;
    int sourceColumn;
};

} // namespace parser
} // namespace bl

namespace std
{
template<>
struct hash<bl::parser::Node::Sequence> {
    std::size_t operator()(const bl::parser::Node::Sequence& key) const {
        std::size_t seed = key.size();
        for (const auto& i : key) {
            seed ^= static_cast<std::size_t>(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

} // namespace std

inline bool operator==(const bl::parser::Node::Sequence& rhs,
                       const bl::parser::Node::Sequence& lhs) {
    if (rhs.size() != lhs.size()) return false;
    for (unsigned int i = 0; i < rhs.size(); ++i) {
        if (rhs[i] != lhs[i]) return false;
    }
    return true;
}

#endif
