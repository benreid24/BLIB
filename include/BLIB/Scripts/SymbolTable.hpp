#ifndef BLIB_SCRIPTS_SYMBOLTABLE_HPP
#define BLIB_SCRIPTS_SYMBOLTABLE_HPP

#include <BLIB/Scripts/Function.hpp>
#include <BLIB/Scripts/Value.hpp>
#include <list>
#include <string>
#include <unordered_map>

namespace bl
{
namespace scripts
{
/**
 * @brief Stack aware symbol table for managing functions and variables
 *        Each Script holds a single instance
 * @ingroup Scripts
 *
 */
class SymbolTable {
public:
    /**
     * @brief Creates the global stack frame
     *
     */
    SymbolTable();

    /**
     * @brief Pushes a new stack frame on
     *
     */
    void pushFrame();

    /**
     * @brief Pops off the top stack frame. Will never pop the global frame
     *
     */
    void popFrame();

    /**
     * @brief Tells whether the given symbol exists or not
     *
     * @param name Name of the symbol to search for
     */
    bool exists(const std::string& name) const;

    /**
     * @brief Returns a Value from the current stack frame or above
     *
     * @param name The name of the Value to get
     * @return Value::Ptr The Value or nullptr on error
     */
    Value::Ptr get(const std::string& name);

    /**
     * @brief Sets a Value in the table in the current frame
     *
     * @param name Name of the symbol to set or create
     * @param value The value to store in the table
     */
    void set(const std::string& name, const Value& value); // TODO - allow force top

private:
    std::vector<std::unordered_map<std::string, Value::Ptr>> table;
};

} // namespace scripts

} // namespace bl

#endif