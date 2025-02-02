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
class World;
} // namespace engine

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

    /// How the component responds to mouse over state
    enum struct HighlightState { IgnoresMouse, HighlightedByMouse };

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
     * @brief Overrides the default highlight behavior for this specific component
     *
     * @param behavior How to behave when moused over
     */
    void overrideHighlightBehavior(HighlightState behavior);

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
     */
    void onAcquisition();

    /**
     * @brief Called when the element is moved
     */
    void onMove();

    /**
     * @brief Called when element state changes. This is element specific, but can mean toggle
     *        state, contents, etc. Also called following create()
     */
    virtual void onElementUpdated() = 0;

    /**
     * @brief Called when the owning elements render settings are changed and after creation
     */
    virtual void onRenderSettingChange() = 0;

    /**
     * @brief Derived classes should return the ECS entity that children should parent themselves to
     */
    virtual ecs::Entity getEntity() const = 0;

    /**
     * @brief Allows GUI elements to query the size required for the visual component
     */
    virtual sf::Vector2f getRequisition() const;

    /**
     * @brief Called when the depth of the component is modified. Default implementation fetches the
     *        com::Transform2D for the entity returned by getEntity() and sets it there
     *
     * @param depth The depth to set to
     */
    virtual void assignDepth(float depth);

protected:
    /**
     * @brief Initializes the component
     *
     * @param highlightState How the component should respond to mouse events
     */
    Component(HighlightState highlightState);

    /**
     * @brief Called once before use. Derived classes should create resources here
     *
     * @param world The world to create entities in
     * @param renderer The GUI renderer instance
     */
    virtual void doCreate(engine::World& world, Renderer& renderer) = 0;

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
     * @brief Called when the relative position or size of this component changes and after creation
     */
    virtual void handleAcquisition() = 0;

    /**
     * @brief Called when the element is moved
     */
    virtual void handleMove() = 0;

    /**
     * @brief Type safe method to get the owning GUI Element. Throws if the type is incorrect
     *
     * @tparam T The derived Element type to cast to
     * @return A reference to the owning Element
     */
    template<typename T>
    T& getOwnerAs() {
        T* o = dynamic_cast<T*>(owner);
        if (!o) {
            BL_LOG_ERROR << "Expected owner of type " << typeid(T).name();
            throw std::runtime_error("Bad owner cast");
        }
        return *o;
    }

    /**
     * @brief Type safe method to get the owning GUI Element. Throws if the type is incorrect
     *
     * @tparam T The derived Element type to cast to
     * @return A reference to the owning Element
     */
    template<typename T>
    const T& getOwnerAs() const {
        const T* o = dynamic_cast<const T*>(owner);
        if (!o) {
            BL_LOG_ERROR << "Expected owner of type " << typeid(T).name();
            throw std::runtime_error("Bad owner cast");
        }
        return *o;
    }

    /**
     * @brief Returns the parent component of this component. May be nullptr
     */
    Component* getParent() const { return parent; }

    /**
     * @brief Helper method for components containing other components to call doSceneAdd on their
     *        children
     *
     * @param component The child component to call doSceneAdd on
     * @param overlay The overlay to call with
     */
    static void doSceneAdd(Component& component, rc::Overlay* overlay) {
        component.addToScene(overlay);
    }

    /**
     * @brief Helper method for components containing other components to call doSceneRemove on
     *        their children
     *
     * @param component The child component to call doSceneRemove on
     */
    static void doSceneRemove(Component& component) { component.doSceneRemove(); }

    /**
     * @brief Helper method for components containing other components to call create on
     *        their children
     *
     * @param component The child component to call create on
     */
    void createChild(Component& child) { child.create(*worldPtr, *renderer, *owner); }

private:
    HighlightState highlightState;
    engine::World* worldPtr;
    Renderer* renderer;
    Element* owner;
    Component* parent;
    UIState state;
    sf::FloatRect priorAcq;

    void create(engine::World& world, Renderer& renderer, Element& owner);
    void addToScene(rc::Overlay* overlay);

    friend class Renderer;
};

} // namespace rdr
} // namespace gui
} // namespace bl

#endif
