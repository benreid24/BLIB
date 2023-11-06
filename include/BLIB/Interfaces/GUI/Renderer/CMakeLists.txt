target_sources(BLIB PUBLIC
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
    TooltipProvider.hpp
)

add_subdirectory(Basic)
