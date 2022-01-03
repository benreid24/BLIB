#ifndef BLIB_SCRIPTS_SYMBOLTABLE_HPP
#define BLIB_SCRIPTS_SYMBOLTABLE_HPP

#include <BLIB/Scripts/Function.hpp>
#include <BLIB/Scripts/Value.hpp>
#include <BLIB/Util/Waiter.hpp>
#include <atomic>
#include <condition_variable>
#include <list>
#include <mutex>
#include <string>
#include <unordered_map>

namespace bl
{
namespace script
{
class Manager;

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
     * @brief Copies the symbol table
     *
     */
    SymbolTable(const SymbolTable& copy);

    /**
     * @brief Copies the symbol table
     *
     */
    void copy(const SymbolTable& copy);

    /**
     * @brief Returns a new SymbolTable that inherits the global scope of this one
     *
     */
    SymbolTable base() const;

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
     * @brief Returns the current stack depth. Used for validating references
     *
     * @return int
     */
    int currentDepth() const;

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
     * @return Value::Ref The Value or nullptr on error
     */
    Value::Ref get(const std::string& name, bool create = false);

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
     * @brief Gets (or optionally creates) a property on a value
     *
     * @param ref The value to get the property from
     * @param name The name of the property
     * @param create True to create if doesn't exist, false to fail
     * @return Value* Address of the property or nullptr
     */
    Value* getProp(const Value::Ref& ref, const std::string& name, bool create = false);

    /**
     * @brief Sets the value of a property on the given value
     *
     * @param ref The value to set the property on
     * @param name The name of the property to set
     * @param value The value to set it to
     * @return Value* Address of the created property
     */
    Value* setProp(const Value::Ref& ref, const std::string& name, const Value& value);

    /**
     * @brief Returns all properties on the given ref
     *
     * @param ref The value to get properties on
     * @return const std::unordered_map<std::string, Value>& All properties on the value
     */
    const std::unordered_map<std::string, Value>& getAllProps(const Value& val) const;

    /**
     * @brief Resets the table to an empty state with an empty global frame
     *
     */
    void reset();

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
     * @brief Registers the Manager controlling this context
     *
     */
    void registerManager(Manager* manager);

    /**
     * @brief Returns the Manager managing this context, nullptr if none
     *
     */
    Manager* manager();

    /**
     * @brief Pauses the current thread for the given amount of time. Unpauses if the script is
     *        terminated during its sleep
     *
     * @param milliseconds The number of milliseconds to pause for
     */
    void waitFor(unsigned long int milliseconds);

    /**
     * @brief Blocks script execution on the given Waiter. Unblocks and exits if the script is
     *        killed while waiting
     *
     * @param waiter The waiter to wait on
     *
     */
    void waitOn(util::Waiter& waiter);

private:
    std::vector<std::unordered_map<std::string, Value>> table;
    std::vector<std::unordered_map<const Value*, std::unordered_map<std::string, Value>>> props;
    Manager* mgr;
    std::atomic<bool> stop;
    std::mutex waitMutex;
    std::condition_variable waitVar;
};

} // namespace script
} // namespace bl

#endif
