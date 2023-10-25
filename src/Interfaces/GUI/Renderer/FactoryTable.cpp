#include <BLIB/Interfaces/GUI/Renderer/FactoryTable.hpp>

namespace bl
{
namespace gui
{
namespace rdr
{
FactoryTable::FactoryTable(bool populate) {
    if (populate) {
        // TODO - create default factories
    }
}

FactoryTable& FactoryTable::getDefaultTable() {
    static FactoryTable table(true);
    return table;
}

} // namespace rdr
} // namespace gui
} // namespace bl
