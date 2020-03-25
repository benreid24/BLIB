#ifndef BLIB_SCRIPTS_SCRIPTIMPL_HPP
#define BLIB_SCRIPTS_SCRIPTIMPL_HPP

#include <BLIB/Scripts.hpp>
#include <optional>

namespace bl
{
namespace scripts
{
/**
 * @brief Collection of testable utility methods for executing components of scripts
 *
 */
struct ScriptImpl {
    /**
     * @brief Takes a Value Node and computes the actual Value
     *
     * @return Value The resulting Value
     */
    static Value computeValue(parser::Node::Ptr value, SymbolTable& table);

    /**
     * @brief Performs a function call. Evaluates arguments, updates SymbolTable, and runs
     * statements
     *
     * @return Value Result of the function called
     */
    static Value runFunction(parser::Node::Ptr call, SymbolTable& table);

    /**
     * @brief Executes a list of statements. Can take Statement, StmtList, or StmtBlock
     *
     * @return std::optional<Value> Result of a return if present, or null if no return
     *
     */
    static std::optional<Value> runStatementList(parser::Node::Ptr statement,
                                                 SymbolTable& table);

    /**
     * @brief Executes a statement
     *
     * @return std::optional<Value> Result of a return if present, or null if no return
     */
    static std::optional<Value> runStatement(parser::Node::Ptr statement, SymbolTable& table);

    /**
     * @brief Runs a Conditional
     *
     * @return std::optional<Value> Value of any executed Return, null if no return
     */
    static std::optional<Value> runConditional(parser::Node::Ptr conditional,
                                               SymbolTable& table);

    /**
     * @brief Runs a Loop
     *
     * @return std::optional<Value> Value of any executed Return, null if no return
     */
    static std::optional<Value> runLoop(parser::Node::Ptr loop, SymbolTable& table);

    /**
     * @brief Evaluates a PGroup to true or false
     *
     */
    static bool evaluateCond(parser::Node::Ptr pgroup, SymbolTable& table);
};

} // namespace scripts
} // namespace bl

#endif