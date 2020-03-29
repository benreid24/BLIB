#ifndef BLIB_SCRIPT_LIBRARY_HPP
#define BLIB_SCRIPT_LIBRARY_HPP

#include <BLIB/Scripts/SymbolTable.hpp>

namespace bl
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
} // namespace bl

#endif