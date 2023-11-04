#include <BLIB/Interfaces/GUI/Renderer/FactoryTable.hpp>

#include <BLIB/Interfaces/GUI.hpp>
#include <BLIB/Interfaces/GUI/Renderer/NullComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/NullFlashProvider.hpp>
#include <BLIB/Interfaces/GUI/Renderer/NullHighlightProvider.hpp>
#include <BLIB/Interfaces/GUI/Renderer/NullTooltipProvider.hpp>

#include <BLIB/Interfaces/GUI/Renderer/Components/BoxComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Components/LabelComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Components/SeparatorComponent.hpp>

namespace bl
{
namespace gui
{
namespace rdr
{
FactoryTable::FactoryTable(bool populate) {
    if (populate) {
        registerFactoryForElement<Animation, NullComponent>();
        registerFactoryForElement<Box, defcoms::BoxComponent>();
        registerFactoryForElement<Button, NullComponent>();
        registerFactoryForElement<Canvas, NullComponent>();
        registerFactoryForElement<CheckButton, NullComponent>();
        registerFactoryForElement<ComboBox, NullComponent>();
        registerFactoryForElement<GUI, NullComponent>();
        registerFactoryForElement<Image, NullComponent>();
        registerFactoryForElement<Label, defcoms::LabelComponent>();
        registerFactoryForElement<Notebook, NullComponent>();
        registerFactoryForElement<ProgressBar, NullComponent>();
        registerFactoryForElement<RadioButton, NullComponent>();
        registerFactoryForElement<ScrollArea, NullComponent>();
        registerFactoryForElement<SelectBox, NullComponent>();
        registerFactoryForElement<Separator, defcoms::SeparatorComponent>();
        registerFactoryForElement<Slider, NullComponent>();
        registerFactoryForElement<TextEntry, NullComponent>();
        registerFactoryForElement<ToggleButton, NullComponent>();
        registerFactoryForElement<Window, NullComponent>();

        setFlashProviderFactory([]() { return std::make_unique<NullFlashProvider>(); });
        setHighlightProviderFactory([]() { return std::make_unique<NullHighlightProvider>(); });
        setTooltipProviderFactory([]() { return std::make_unique<NullTooltipProvider>(); });
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
