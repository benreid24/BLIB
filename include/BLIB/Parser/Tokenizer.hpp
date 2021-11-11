#ifndef BLIB_PARSER_TOKENIZER_HPP
#define BLIB_PARSER_TOKENIZER_HPP

#include <BLIB/Parser/ISkipper.hpp>
#include <BLIB/Parser/Node.hpp>

#include <list>
#include <regex>
#include <unordered_map>
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
    enum MatchGroup : unsigned int {
        EntireMatch = 0,
        Group1      = 1,
        Group2      = 2,
        Group3      = 3,
        Group4      = 4,
        Group5      = 5
    };

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
     * @param matchGroup Which match group to use for the token data. Defaults to entire match
     */
    void addTokenType(Node::Type type, const std::string& regex,
                      MatchGroup matchGroup = EntireMatch);

    /**
     * @brief Specify a character that disables/enables the skipper when encountered
     *        Example: '"' for strings
     *
     * @param c A character to enable/disable the skipper
     */
    void addSkipperToggleChar(char c);

    /**
     * @brief Adds an escape sequence that replaces matching substrings of the Node.data with c
     *        Note that this will not affect parsing in the case of something
     *        like "string \" literal". The token matcher will have to handle that
     *
     * @param sequence The sequence to search for in token data
     * @param c The replacement character
     */
    void addEscapeSequence(const std::string& sequence, char c);

    /**
     * @brief Set exact matches of a particular string of one node type to map to another type
     *        Meant to do transformations from general id tokens to specific keywords
     *
     * @param rootType Node type to check exact matchs from
     * @param kwordType Node type to map to
     * @param kword Keyword to perform the transformation on
     */
    void addKeyword(Node::Type rootType, Node::Type kwordType, const std::string& kword);

    /**
     * @brief Parses the input stream into a token list
     *
     * @param input Stream to parse
     * @param err Optional string to populate with error message
     * @return std::vector<Node::Ptr> List of parsed tokens, empty on error
     */
    std::vector<Node::Ptr> tokenize(Stream& input, std::string* err = nullptr) const;

private:
    ISkipper::Ptr skipper;
    std::unordered_map<std::string, std::pair<std::regex, Node::Type>> matchers;
    std::unordered_map<std::string, unsigned int> matchGroups;
    std::unordered_map<std::string, std::list<std::string>> ambiguous;
    std::unordered_map<Node::Type, std::vector<std::pair<std::string, Node::Type>>> kwords;
    std::vector<char> togglers;
    std::vector<std::pair<std::string, char>> escapeSequences;

    void recomputeAmbiguous();
};

} // namespace parser

} // namespace bl

#endif
