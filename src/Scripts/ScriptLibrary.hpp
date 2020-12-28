#ifndef BLIB_SCRIPT_LIBRARY_HPP
#define BLIB_SCRIPT_LIBRARY_HPP

#include <BENG/Scripts/SymbolTable.hpp>

namespace bg
{
namespace scripts
{
/**
 * @brief Static helper to add built-in functions to each script
 * @ingroup Scripts
 */
struct Library {
    static void addBuiltIns(SymbolTable& table);
};

} // namespace scripts
} // namespace bg

#endif