#ifndef BLIB_GUI_RENDERER_COMPONENT_HPP
#define BLIB_GUI_RENDERER_COMPONENT_HPP

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
    virtual void setUIState(UIState state) = 0;

    /**
     * @brief Call to display a tooltip for this component
     *
     * @param text The tooltip text
     * @param position The triggering mouse position in overlay coordinates
     */
    void showTooltip(const std::string& text, const glm::vec2& position);

protected:
    /**
     * @brief Initializes the component
     */
    Component();

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
    Renderer* renderer;
    Element* owner;
    UIState state;

    void create(engine::Engine& engine, Renderer& renderer, Element& owner, Component& windowOrGui);

    friend class Renderer;
};

} // namespace rdr
} // namespace gui
} // namespace bl

#endif
