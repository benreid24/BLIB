#ifndef BLIB_PARSER_GRAMMAR_HPP
#define BLIB_PARSER_GRAMMAR_HPP

#include <BLIB/Parser/Tokenizer.hpp>
#include <unordered_map>

namespace bl
{
namespace parser
{
/**
 * @brief Represents a grammar for a Parser to build a parse tree with from a token sequence
 *
 */
class Grammar {
public:
    /**
     * @brief Set the Start Node type
     *
     * @param start The root node type of the grammar
     */
    void setStart(Node::Type start);

    /**
     * @brief Adds a grammar rule
     *
     * @param result The non-terminal Node to create
     * @param elements The list of Nodes that constitute the non-terminal
     */
    void addRule(Node::Type result, const Node::Sequence& elements);

    /**
     * @brief Convenience rule construction
     *
     * @param result The non-terminal to generate
     * @param rhs The terminal or non-terminal node to reduce from
     */
    void addRule(Node::Type result, Node::Type rhs);

    /**
     * @brief Builds the reduction table from the list of rules
     *
     */
    void compile();

    /**
     * @brief Helper struct to represent an entry in the reduction table. A rule sequence maps
     *        onto a Reduction value
     * @ingroup Parser
     */
    struct Reduction {
        Node::Type result;
        unsigned int reductionsPossible;
    };

    /**
     * @brief Performs a lookup in the reductions table
     *
     * @param sequence The sequence of nodes to search for
     * @return Reduction The result of the reduction lookup
     */
    Reduction reductionLookup(const Node::Sequence& sequence) const;

private:
    Node::Type start;
    std::vector<std::pair<Node::Type, Node::Sequence>> rules;
    std::unordered_map<Node::Sequence, Reduction> reductionTable;
};

} // namespace parser

} // namespace bl

#endif