#ifndef BLIB_SCRIPTS_CONTEXT_HPP
#define BLIB_SCRIPTS_CONTEXT_HPP

#include <BLIB/Scripts/SymbolTable.hpp>

namespace bl
{
namespace script
{
class Script;

/**
 * @brief A context to run a script with. Custom context classes may be created to have different
 *        types of scripts. For example, an NPC running a script may have a context that adds
 *        several global functions to manipulate that NPC directly from the script
 *
 * @ingroup Scripts
 *
 */
class Context {
public:
    /**
     * @brief Destroy the Context object
     *
     */
    virtual ~Context() = default;

protected:
    /**
     * @brief Add your custom functions and variables to the symbol table here
     *
     * @param table The symbol table that will be used by any script using this context
     */
    virtual void addCustomSymbols(SymbolTable& table) const;

private:
    void initializeTable(SymbolTable& table) const;

    friend class Script;
};

} // namespace script
} // namespace bl

#endif
