#ifndef BLIB_GUI_RENDERER_COMPONENTFACTORY_HPP
#define BLIB_GUI_RENDERER_COMPONENTFACTORY_HPP

#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>
#include <type_traits>

namespace bl
{
namespace gui
{
namespace rdr
{
/**
 * @brief Base class for component factories. Component factories are owned by FactoryTables, which
 *        are used by GUI to create Components to render Elements
 *
 * @ingroup GUI
 */
struct ComponentFactory {
    using Ptr = std::unique_ptr<ComponentFactory>;

    /**
     * @brief Destroys the factory
     */
    virtual ~ComponentFactory() = default;

    /**
     * @brief Called when a Component needs to be created
     */
    virtual Component::Ptr create() = 0;
};

/**
 * @brief Generic factory for components with simple constructors. Provided for convenience
 *
 * @tparam T The type of Component to create
 * @ingroup GUI
 */
template<typename T>
struct GenericFactory : public ComponentFactory {
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

    /**
     * @brief Destroys the factory
     */
    virtual ~GenericFactory() = default;

    /**
     * @brief Creates a Component of type T
     */
    virtual Component::Ptr create() override { return std::make_unique<T>(); }
};

} // namespace rdr
} // namespace gui
} // namespace bl

#endif
