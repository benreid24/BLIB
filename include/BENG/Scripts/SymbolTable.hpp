#ifndef BLIB_SCRIPTS_SYMBOLTABLE_HPP
#define BLIB_SCRIPTS_SYMBOLTABLE_HPP

#include <BENG/Scripts/Function.hpp>
#include <BENG/Scripts/Value.hpp>
#include <list>
#include <string>
#include <unordered_map>

namespace bg
{
class ScriptManager;
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
    Value::Ptr get(const std::string& name, bool create = false);

    /**
     * @brief Sets a Value in the table in the current frame
     *
     * @param name Name of the symbol to set or create
     * @param value The value to store in the table
     * @param forceTop If true will create the variable in the current scope even if it exists
     *                 in higher scope
     */
    void set(const std::string& name, const Value& value, bool forceTop = false);

    /**
     * @brief Sets the internal kill flag to terminate script execution
     *
     */
    void kill();

    /**
     * @brief Returns the value of the internal kill flag
     *
     */
    bool killed() const;

    /**
     * @brief Registers the ScriptManager controlling this context
     *
     */
    void registerManager(ScriptManager* manager);

    /**
     * @brief Returns the ScriptManager managing this context, nullptr if none
     *
     */
    ScriptManager* manager();

private:
    std::vector<std::unordered_map<std::string, Value::Ptr>> table;
    ScriptManager* mgr;
    bool stop;
};

} // namespace scripts

} // namespace bg

#endif