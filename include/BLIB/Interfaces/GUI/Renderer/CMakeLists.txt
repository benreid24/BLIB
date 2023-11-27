target_sources(BLIB PUBLIC
    CanvasComponentBase.hpp
    Component.hpp
    ComponentFactory.hpp
    FactoryTable.hpp
    FlashProvider.hpp
    HighlightProvider.hpp
    NullComponent.hpp
    NullFlashProvider.hpp
    NullHighlightProvider.hpp
    NullTooltipProvider.hpp
    Renderer.hpp
    TextEntryComponentBase.hpp
    TooltipProvider.hpp
)

add_subdirectory(Basic)
