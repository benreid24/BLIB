#ifndef BLIB_SCRIPT_LIBRARY_HPP
#define BLIB_SCRIPT_LIBRARY_HPP

#include <BLIB/Scripts/SymbolTable.hpp>

namespace bl
{
namespace script
{
/**
 * @brief Static helper to add built-in functions to each script
 * @ingroup Scripts
 */
struct Library {
    static void addBuiltIns(SymbolTable& table);
};

} // namespace script
} // namespace bl

#endif