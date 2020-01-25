#ifndef BLIB_PARSER_TOKENIZER_HPP
#define BLIB_PARSER_TOKENIZER_HPP

#include <BLIB/Parser/ISkipper.hpp>
#include <BLIB/Parser/Node.hpp>

#include <list>
#include <regex>
#include <vector>

namespace bl
{
namespace parser
{
/**
 * @brief Utility class for a Parser to break down an input string into tokens
 * @ingroup Parser
 *
 */
class Tokenizer {
public:
    /**
     * @brief Construct a new Tokenizer object
     *
     * @param skipper The skipper to use. Can be null
     */
    Tokenizer(ISkipper::Ptr skipper);

    /**
     * @brief Adds a node matcher to the tokenizer. It is important to note that token types
     *        are considered in the order they are added
     *
     * @param type Id of the token to create if a match is found
     * @param regex Regex to match the token. Token data will be whole match, or first group if
     *              present
     */
    void addTokenType(Node::Type type, const std::string& regex);

    /**
     * @brief Parses the input stream into a token list
     *
     * @param input Stream to parse
     * @return std::vector<Node::Ptr> List of parsed tokens, empty on error
     */
    std::vector<Node::Ptr> tokenize(Stream& input) const;

private:
    ISkipper::Ptr skipper;
    std::list<std::pair<std::regex, Node::Type>> matchers;
};

} // namespace parser

} // namespace bl

#endif