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

    /**
     * @brief Register a new Node type as a terminal
     *
     * @param terminal The terminal symbol
     */
    void addTerminal(Node::Type terminal);

    /**
     * @brief Register a new Node type as a nonterminal
     *
     * @param nonterminal The nonterminal symbol
     */
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

    /**
     * @brief Get the start symbol
     *
     */
    Node::Type getStart() const;

    /**
     * @brief Helper struct to represent grammar productions
     *
     */
    struct Production {
        Node::Type result;
        Node::Sequence set;

        bool operator==(const Production& rhs) const;
    };

    /**
     * @brief A Production with a cursor representing current parser position
     *
     */
    struct Item {
        Production production;
        unsigned int cursor;

        Item(const Production& copy);
        Item(const Production& copy, unsigned int c);

        bool operator==(const Item& rhs) const;
        bool final() const;
        Item next() const;
    };

    /**
     * @brief A set of unique Items
     *
     */
    class ItemSet {
    public:
        bool contains(const Item& item) const;
        bool add(const Item& item);
        const std::list<Item>& items() const;
        bool operator==(const ItemSet& rhs) const;

    private:
        std::list<Item> set;
    };

    /**
     * @brief Returns true if the symbol is a terminal
     *
     */
    bool terminal(Node::Type t) const;

    /**
     * @brief Returns true if the symbol is a nonterminal
     *
     */
    bool nonterminal(Node::Type t) const;

    /**
     * @brief Returns the follow set of the given symbol
     *
     * @param t Symbol to determine follow set of
     * @param recurseGuard Internal implementation only
     * @return Node::Sequence The terminal symbols that can follow t
     */
    Node::Sequence followSet(Node::Type t, Node::Sequence* recurseGuard = nullptr) const;

    /**
     * @brief Computes the closure of an Item
     *
     * @param item Item to take the closure of
     * @param result Internal implementation only
     * @return ItemSet The closure
     */
    ItemSet closure(const Item& item, ItemSet* result = nullptr) const;

    /**
     * @brief Returns the union of the closures of each Item in the set
     *
     * @param itemSet Items to take the closure of
     * @return ItemSet Union of all closures
     */
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