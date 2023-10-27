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

void FactoryTable::setFlashProviderFactory(FlashProvider::Factory&& factory) {
    flashFactory = std::move(factory);
}

void FactoryTable::setHighlightProviderFactory(HighlightProvider::Factory&& factory) {
    highlightFactory = std::move(factory);
}

void FactoryTable::setTooltipProviderFactory(TooltipProvider::Factory&& factory) {
    tooltipFactory = std::move(factory);
}

FlashProvider::Ptr FactoryTable::createFlashProvider() const { return flashFactory(); }

HighlightProvider::Ptr FactoryTable::createHighlightProvider() const { return highlightFactory(); }

TooltipProvider::Ptr FactoryTable::createTooltipProvider() const { return tooltipFactory(); }

} // namespace rdr
} // namespace gui
} // namespace bl
