#ifndef BLIB_PARSER_GRAMMAR_HPP
#define BLIB_PARSER_GRAMMAR_HPP

#include <BLIB/Parser/Tokenizer.hpp>
#include <unordered_map>
#include <unordered_set>

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

    void addTerminal(Node::Type terminal);
    void addNonTerminal(Node::Type nonterminal);

    /**
     * @brief Adds a grammar rule
     *
     * @param result The non-terminal Node to create
     * @param elements The list of Nodes that constitute the non-terminal
     */
    bool addRule(Node::Type result, const Node::Sequence& elements);

    /**
     * @brief Convenience rule construction
     *
     * @param result The non-terminal to generate
     * @param rhs The terminal or non-terminal node to reduce from
     */
    bool addRule(Node::Type result, Node::Type rhs); // TODO - test add fail

    Node::Type getStart() const;

    struct Production {
        Node::Type result;
        Node::Sequence set;

        bool operator==(const Production& rhs) const;
    };

    struct Item {
        Production production;
        unsigned int cursor;

        Item(const Production& copy);
        Item(const Production& copy, int c);

        bool operator==(const Item& rhs) const;
        bool final() const;
        Item next() const;
    };

    class ItemSet {
    public:
        bool contains(const Item& item) const;
        bool add(const Item& item);
        const std::list<Item>& items() const;
        bool operator==(const ItemSet& rhs) const;

    private:
        std::list<Item> set;
    };

    bool terminal(Node::Type t) const;
    bool nonterminal(Node::Type t) const;
    Node::Sequence followSet(Node::Type t, Node::Sequence* recurseGuard = nullptr) const;

    ItemSet closure(const Item& item, ItemSet* result = nullptr) const;
    ItemSet closure(const ItemSet& itemSet) const;

private:
    Node::Type start;
    std::unordered_set<Node::Type> terminals;
    std::unordered_set<Node::Type> nonterminals;
    std::vector<Production> productions;

    std::list<Production> producing(Node::Type t) const;
    Node::Sequence firstSet(Node::Type t, Node::Sequence& recurseGuard) const;
};

} // namespace parser

} // namespace bl

#endif