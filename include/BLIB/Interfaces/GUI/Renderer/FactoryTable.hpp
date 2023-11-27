#ifndef BLIB_GUI_RENDERER_FACTORYTABLE_HPP
#define BLIB_GUI_RENDERER_FACTORYTABLE_HPP

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>
#include <BLIB/Interfaces/GUI/Renderer/ComponentFactory.hpp>
#include <BLIB/Interfaces/GUI/Renderer/FlashProvider.hpp>
#include <BLIB/Interfaces/GUI/Renderer/HighlightProvider.hpp>
#include <BLIB/Interfaces/GUI/Renderer/TooltipProvider.hpp>
#include <typeindex>
#include <unordered_map>

namespace bl
{
namespace gui
{
namespace rdr
{
/**
 * @brief Managed mapping of GUI Element to ComponentFactory. Allows swapping out render behavior
 *        for specific Element types individually
 *
 * @ingroup GUI
 */
class FactoryTable {
public:
    /**
     * @brief Creates a new factory table
     *
     * @param populateWithDefaults True to populate with the default factories, false to leave empty
     */
    FactoryTable(bool populateWithDefaults = true);

    /**
     * @brief Returns the global default table. GUI's use this table unless overridden
     */
    static FactoryTable& getDefaultTable();

    /**
     * @brief Sets the callback to use to create flash providers
     *
     * @param factory The factory callback
     */
    void setFlashProviderFactory(FlashProvider::Factory&& factory);

    /**
     * @brief Sets the callback to use to create highlight providers
     *
     * @param factory The factory callback
     */
    void setHighlightProviderFactory(HighlightProvider::Factory&& factory);

    /**
     * @brief Sets the callback to use to create tooltip providers
     *
     * @param factory The factory callback
     */
    void setTooltipProviderFactory(TooltipProvider::Factory&& factory);

    /**
     * @brief Adds a factory to the table
     *
     * @tparam TElem The element type to register the factory for
     * @param factory The factory to use for the given element type
     */
    template<typename TElem>
    void registerFactoryForElement(ComponentFactory::Ptr&& factory) {
        static_assert(std::is_base_of_v<Element, TElem>, "TElem must be derived from Element");
        table[typeid(TElem)] = std::move(factory);
    }

    /**
     * @brief Convenience method to use the GenericFactory for the given element
     *
     * @tparam TElem The element type to use the GenericFactory for
     * @tparam TCom The Component type to create for elements of type TElem
     */
    template<typename TElem, typename TCom>
    void registerFactoryForElement() {
        static_assert(std::is_base_of_v<Element, TElem>, "TElem must be derived from Element");
        table[typeid(TElem)] = std::make_unique<GenericFactory<TCom>>();
    }

    /**
     * @brief Uses the registered factory to create a Component for the given element type
     *
     * @tparam TElem The Element type to create the Component for
     * @return The created component. Throws if the table does not have the correct factory
     */
    template<typename TElem>
    Component::Ptr createComponent() const {
        const auto it = table.find(typeid(TElem));
        if (it == table.end()) {
            BL_LOG_ERROR << "Failed to find Component creator for Element: "
                         << typeid(TElem).name();
            throw std::runtime_error("Missing ComponentFactory");
        }
        return it->second->create();
    }

    /**
     * @brief Creates a flash provider
     */
    FlashProvider::Ptr createFlashProvider() const;

    /**
     * @brief Creates a highlight provider
     */
    HighlightProvider::Ptr createHighlightProvider() const;

    /**
     * @brief Creates a tooltip provider
     */
    TooltipProvider::Ptr createTooltipProvider() const;

private:
    std::unordered_map<std::type_index, ComponentFactory::Ptr> table;
    FlashProvider::Factory flashFactory;
    HighlightProvider::Factory highlightFactory;
    TooltipProvider::Factory tooltipFactory;
};

} // namespace rdr
} // namespace gui
} // namespace bl

#endif
