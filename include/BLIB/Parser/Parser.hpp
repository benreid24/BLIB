#ifndef BLIB_PARSER_PARSER_HPP
#define BLIB_PARSER_PARSER_HPP

#include <BLIB/Parser/Grammar.hpp>
#include <BLIB/Parser/Tokenizer.hpp>

namespace bl
{
/**
 * @brief Top level Parser class. Implements a basic LR0 parser from a Grammar and Tokenizer
 * @ingroup Parser
 *
 */
class Parser {
public:
    /**
     * @brief Construct a new Parser object
     *
     * @param grammar The Grammar to use
     * @param tokenizer The Tokenizer to use
     */
    Parser(const parser::Grammar& grammar, const parser::Tokenizer& tokenizer);

    /**
     * @brief Parses the input stream into a parse tree of Nodes
     *
     * @param input The stream to tokenize and parse
     * @return parser::Node::Ptr The root of the tree, or nullptr if error
     */
    parser::Node::Ptr parse(parser::Stream& input) const;

    /**
     * @brief Convenience method to avoid manually creating a Stream
     * @see parser::Node::Ptr parse(parser::Stream& input)
     *
     * @param input Input to parse into tree
     * @return parser::Node::Ptr The generated parse tree or nullptr on error
     */
    parser::Node::Ptr parse(const std::string& input) const;

private:
    const parser::Grammar grammar;
    const parser::Tokenizer tokenizer;

    bool tryReduction(std::vector<parser::Node::Ptr>& stack, bool inputRemaining,
                      bool unchanged) const;
};

} // namespace bl

#endif