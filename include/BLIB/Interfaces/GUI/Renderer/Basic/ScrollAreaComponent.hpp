#ifndef BLIB_GUI_RENDERER_COMPONENTS_SCROLLAREACOMPONENT_HPP
#define BLIB_GUI_RENDERER_COMPONENTS_SCROLLAREACOMPONENT_HPP

#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
/**
 * @brief Default component type for ScrollArea elements
 *
 * @ingroup GUI
 */
class ScrollAreaComponent : public rdr::Component {
public:
    /**
     * @brief Creates the component
     */
    ScrollAreaComponent();

    /**
     * @brief Destroys the component
     */
    virtual ~ScrollAreaComponent() = default;

    /**
     * @brief Toggle the visibility of the UI component
     *
     * @param visible True to be rendered, false to hide
     */
    virtual void setVisible(bool visible) override;

    /**
     * @brief Called when element state changes. This is element specific, but can mean toggle
     *        state, contents, etc.
     */
    virtual void onElementUpdated() override;

    /**
     * @brief Called when the owning elements render settings are changed
     */
    virtual void onRenderSettingChange() override;

    /**
     * @brief Derived classes should return the ECS entity that children should parent themselves to
     */
    virtual ecs::Entity getEntity() const override;

protected:
    /**
     * @brief Creates the component
     *
     * @param engine The game engine instance
     * @param renderer The GUI renderer instance
     */
    virtual void doCreate(engine::Engine& engine, rdr::Renderer& renderer) override;

    /**
     * @brief Adds the component to the scene
     *
     * @param overlay The scene to add to
     */
    virtual void doSceneAdd(rc::Overlay* overlay) override;

    /**
     * @brief Removes the component from the scene
     */
    virtual void doSceneRemove() override;

    /**
     * @brief Configures the component for the new acquisition
     */
    virtual void handleAcquisition() override;

    /**
     * @brief Positions the component when it should move
     */
    virtual void handleMove() override;

private:
    gfx::Rectangle box;
};

} // namespace defcoms
} // namespace gui
} // namespace bl

#endif
