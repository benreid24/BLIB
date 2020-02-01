#ifndef BLIB_SCRIPTS_PARSER_HPP
#define BLIB_SCRIPTS_PARSER_HPP

#include <BLIB/Parser/Node.hpp>

namespace bl
{
namespace scripts
{
/**
 * @brief Implementation of a Parser for the scripting language
 * @ingroup Scripts
 *
 */
struct Parser {
    /**
     * @brief Parses the input script into a parse tree
     *
     * @param input The script to parse
     * @return parser::Node::Ptr The root node, or nullptr on error
     */
    parser::Node::Ptr parse(const std::string& input);

    enum Grammar : parser::Node::Type {
        // TODO - node types
    };
};

} // namespace scripts
} // namespace bl

#endif