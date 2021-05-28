#include <BLIB/Scripts/Context.hpp>
#include <Scripts/ScriptLibrary.hpp>

namespace bl
{
namespace script
{
void Context::initializeTable(SymbolTable& table) const {
    Library::addBuiltIns(table);
    addCustomSymbols(table);
}

} // namespace script
} // namespace bl
