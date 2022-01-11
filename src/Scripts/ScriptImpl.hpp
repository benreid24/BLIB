#ifndef BLIB_SCRIPTS_SCRIPTIMPL_HPP
#define BLIB_SCRIPTS_SCRIPTIMPL_HPP

#include <BLIB/Scripts.hpp>
#include <optional>

namespace bl
{
namespace script
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
    static PrimitiveValue computeValue(const parser::Node::Ptr& value, SymbolTable& table);

    /**
     * @brief Performs a function call. Evaluates arguments, updates SymbolTable, and runs
     * statements
     *
     * @return Value Result of the function called
     */
    static Value runFunction(const parser::Node::Ptr& call, SymbolTable& table);

    /**
     * @brief Executes a list of statements. Can take Statement, StmtList, or StmtBlock
     *
     * @return std::optional<Value> Result of a return if present, or null if no return
     *
     */
    static std::optional<Value> runStatementList(const parser::Node::Ptr& statement,
                                                 SymbolTable& table);

    /**
     * @brief Executes a statement
     *
     * @return std::optional<Value> Result of a return if present, or null if no return
     */
    static std::optional<Value> runStatement(const parser::Node::Ptr& statement,
                                             SymbolTable& table);

    /**
     * @brief Runs a Conditional
     *
     * @return std::optional<Value> Value of any executed Return, null if no return
     */
    static std::optional<Value> runConditional(const parser::Node::Ptr& conditional,
                                               SymbolTable& table);

    /**
     * @brief Runs a Loop
     *
     * @return std::optional<Value> Value of any executed Return, null if no return
     */
    static std::optional<Value> runLoop(const parser::Node::Ptr& loop, SymbolTable& table);

    /**
     * @brief Runs a ForLoop
     *
     * @return std::optional<Value> Value of any executed Return, null if no return
     */
    static std::optional<Value> runForLoop(const parser::Node::Ptr& loop, SymbolTable& table);

    /**
     * @brief Evaluates a PGroup to true or false
     *
     */
    static bool evaluateCond(const parser::Node::Ptr& pgroup, SymbolTable& table);

    /**
     * @brief Helper function that exposes the ability to compare values
     *
     * @param left Left value to compare
     * @param right Right value to compare
     * @return True if values are equal, false if not equal
     */
    static bool equals(const Value& left, const Value& right);
};

} // namespace script
} // namespace bl

#endif
