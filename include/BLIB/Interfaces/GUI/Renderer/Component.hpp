#ifndef BLIB_GUI_RENDERER_COMPONENT_HPP
#define BLIB_GUI_RENDERER_COMPONENT_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <memory>

namespace bl
{
namespace engine
{
class Engine;
}

namespace gui
{
class Element;

/// GUI renderer classes
namespace rdr
{
class Renderer;

/**
 * @brief Base class for renderer components. Each GUI element owns a handle to a component owned by
 *        the renderer. Components are created by factories registered in the GUI's FactoryTable
 *
 * @ingroup GUI
 */
class Component {
public:
    using Ptr = std::unique_ptr<Component>;

    /// Visible state of the element
    enum struct UIState { Regular, Highlighted, Pressed, Disabled };

    /**
     * @brief Destroys the component
     */
    virtual ~Component() = default;

    /**
     * @brief Called when the visual state changes and immediately following creation
     *
     * @param state The visual state of the element
     */
    void setUIState(UIState state);

    /**
     * @brief Toggle the visibility of the UI component
     *
     * @param visible True to be rendered, false to hide
     */
    virtual void setVisible(bool visible) = 0;

    /**
     * @brief Call to display a tooltip for this component
     */
    void showTooltip();

    /**
     * @brief Call to dismiss the tooltip for this component
     */
    void dismissTooltip();

    /**
     * @brief Triggers this component to flash briefly
     */
    void flash();

    /**
     * @brief Called when the element gets its acquisition (re)assigned
     *
     * @param acquisition The acquisition of the element in overlay space
     */
    virtual void onAcquisition(const sf::FloatRect& acquisition) = 0;

    /**
     * @brief Called when element state changes. This is element specific, but can mean toggle
     *        state, contents, etc.
     */
    virtual void onElementUpdated() = 0;

    /**
     * @brief Called when the owning elements render settings are changed
     */
    virtual void onRenderSettingChange() = 0;

    /**
     * @brief Derived classes should return the ECS entity that children should parent themselves to
     */
    virtual ecs::Entity getEntity() const = 0;

protected:
    enum struct HighlightState { IgnoresMouse, HighlightedByMouse };

    /**
     * @brief Initializes the component
     *
     * @param highlightState How the component should respond to mouse events
     */
    Component(HighlightState highlightState);

    /**
     * @brief Called once before use. Derived classes should create resources here
     *
     * @param engine The game engine instance
     * @param renderer The GUI renderer instance
     * @param windowOrGui The Component for the Window or GUI element that is the closest parent
     */
    virtual void doCreate(engine::Engine& engine, Renderer& renderer, Component& windowOrGui) = 0;

    /**
     * @brief Called when the Component should be added to a scene
     *
     * @param overlay The scene to add to
     */
    virtual void doSceneAdd(rc::Overlay* overlay) = 0;

    /**
     * @brief Called when the Component should be removed from the scene
     */
    virtual void doSceneRemove() = 0;

    /**
     * @brief Called when the UI state changes. Derived classes may implement custom handlers here
     *
     * @param src The element changing state
     * @param state The new UI state
     */
    virtual void notifyUIState(UIState state);

    /**
     * @brief Type safe method to get the owning GUI Element. Throws if the type is incorrect
     *
     * @tparam T The derived Element type to cast to
     * @return A reference to the owning Element
     */
    template<typename T>
    T& getOwnerAs() {
        T* o = dynamic_cast<T>(owner);
        if (!o) {
            BL_LOG_ERROR << "Expected owner of type " << typeid(T).name();
            throw std::runtime_error("Bad owner cast");
        }
        return *o;
    }

private:
    const HighlightState highlightState;
    Renderer* renderer;
    Element* owner;
    UIState state;

    void create(engine::Engine& engine, Renderer& renderer, Element& owner, Component* windowOrGui);

    friend class Renderer;
};

} // namespace rdr
} // namespace gui
} // namespace bl

#endif
