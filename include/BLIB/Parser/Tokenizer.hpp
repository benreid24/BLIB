#ifndef BLIB_PARSER_TOKENIZER_HPP
#define BLIB_PARSER_TOKENIZER_HPP

#include <BLIB/Parser/ISkipper.hpp>
#include <BLIB/Parser/Node.hpp>

#include <list>
#include <map>
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
     * @brief Parses the input stream into a token list
     *
     * @param input Stream to parse
     * @return std::vector<Node::Ptr> List of parsed tokens, empty on error
     */
    std::vector<Node::Ptr> tokenize(Stream& input) const;

    // TODO - fix tokenizer group parsing
    // TODO - use whitespace or smth to group token strings before checking
    // wrt to above: need to differntiate krwords from ids. maybe check matchs against others
    // or change interface from regex only to regex or lit. or provide a mapping step. so parse
    // id by regex then habve kword list to map ids to kwords

private:
    ISkipper::Ptr skipper;
    std::map<std::string, std::pair<std::regex, Node::Type>> matchers;
    std::map<std::string, std::list<std::string>> ambiguous;
    std::vector<char> togglers;
    std::vector<std::pair<std::string, char>> escapeSequences;

    void recomputeAmbiguous();
};

} // namespace parser

} // namespace bl

#endif