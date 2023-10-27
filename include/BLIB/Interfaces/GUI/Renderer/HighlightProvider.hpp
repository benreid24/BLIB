#ifndef BLIB_GUI_RENDERER_HIGHLIGHTPROVIDER_HPP
#define BLIB_GUI_RENDERER_HIGHLIGHTPROVIDER_HPP

#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>
#include <functional>

namespace bl
{
namespace gui
{
class Element;

namespace rdr
{
/**
 * @brief Base class for providers of element highlights. This provider type handles when elements
 *        are clicked and moused over
 *
 * @ingroup GUI
 */
class HighlightProvider {
public:
    using Ptr     = std::unique_ptr<HighlightProvider>;
    using Factory = std::function<Ptr()>;

    /**
     * @brief Destroys the provider
     */
    virtual ~HighlightProvider() = default;

    /**
     * @brief Updates the provider
     *
     * @param dt Elapsed time in seconds
     */
    virtual void update(float dt) = 0;

    /**
     * @brief Called when an element changes state and needs a new highlight
     *
     * @param element The element changing state
     * @param state The new state of the element
     */
    virtual void notifyUIState(Element* element, Component::UIState state) = 0;

    /**
     * @brief Called once before use. Derived classes should create resources here
     *
     * @param engine The game engine instance
     */
    virtual void doCreate(engine::Engine& engine) = 0;

    /**
     * @brief Called when the provider should be added to a scene
     *
     * @param overlay The scene to add to
     */
    virtual void doSceneAdd(rc::Overlay* overlay) = 0;

    /**
     * @brief Called when the provider should be removed from the scene
     */
    virtual void doSceneRemove() = 0;
};

} // namespace rdr
} // namespace gui
} // namespace bl

#endif
