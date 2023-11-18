#include <BLIB/Interfaces/GUI/Renderer/FactoryTable.hpp>

#include <BLIB/Interfaces/GUI.hpp>
#include <BLIB/Interfaces/GUI/Renderer/NullComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/NullFlashProvider.hpp>
#include <BLIB/Interfaces/GUI/Renderer/NullHighlightProvider.hpp>
#include <BLIB/Interfaces/GUI/Renderer/NullTooltipProvider.hpp>

#include <BLIB/Interfaces/GUI/Renderer/Basic/BasicTooltipProvider.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/BoxComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/ButtonComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/CanvasComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/CheckButtonComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/ComboBoxComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/IconComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/ImageComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/LabelComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/NotebookComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/OverlayFlashProvider.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/OverlayHighlightProvider.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/ProgressBarComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/RadioButtonComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/SeparatorComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/TextEntryComponent.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/WindowComponent.hpp>

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
        registerFactoryForElement<Button, defcoms::ButtonComponent>();
        registerFactoryForElement<Canvas, defcoms::CanvasComponent>();
        registerFactoryForElement<CheckButton, defcoms::CheckButtonComponent>();
        registerFactoryForElement<ComboBox, defcoms::ComboBoxComponent>();
        registerFactoryForElement<GUI, defcoms::BoxComponent>();
        registerFactoryForElement<Icon, defcoms::IconComponent>();
        registerFactoryForElement<Image, defcoms::ImageComponent>();
        registerFactoryForElement<Label, defcoms::LabelComponent>();
        registerFactoryForElement<Notebook, defcoms::NotebookComponent>();
        registerFactoryForElement<ProgressBar, defcoms::ProgressBarComponent>();
        registerFactoryForElement<RadioButton, defcoms::RadioButtonComponent>();
        registerFactoryForElement<ScrollArea, NullComponent>();
        registerFactoryForElement<SelectBox, NullComponent>();
        registerFactoryForElement<Separator, defcoms::SeparatorComponent>();
        registerFactoryForElement<Slider, NullComponent>();
        registerFactoryForElement<TextEntry, defcoms::TextEntryComponent>();
        registerFactoryForElement<Window, defcoms::WindowComponent>();

        setFlashProviderFactory([]() { return std::make_unique<defcoms::OverlayFlashProvider>(); });
        setHighlightProviderFactory(
            []() { return std::make_unique<defcoms::OverlayHighlightProvider>(); });
        setTooltipProviderFactory(
            []() { return std::make_unique<defcoms::BasicTooltipProvider>(); });
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
