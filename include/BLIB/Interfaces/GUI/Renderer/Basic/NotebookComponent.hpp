#ifndef BLIB_GUI_RENDERER_COMPONENTS_NOTEBOOKCOMPONENT_HPP
#define BLIB_GUI_RENDERER_COMPONENTS_NOTEBOOKCOMPONENT_HPP

#include <BLIB/Graphics/BatchRectangle.hpp>
#include <BLIB/Graphics/BatchedShapes2D.hpp>
#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
/**
 * @brief Default component type for Box elements
 *
 * @ingroup GUI
 */
class NotebookComponent : public rdr::Component {
public:
    /**
     * @brief Creates the component
     */
    NotebookComponent();

    /**
     * @brief Destroys the component
     */
    virtual ~NotebookComponent() = default;

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
     * @param world The world to create entities in
     * @param renderer The GUI renderer instance
     */
    virtual void doCreate(engine::World& world, rdr::Renderer& renderer) override;

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
    gfx::BatchedShapes2D tabBoxBatch;
    std::vector<std::unique_ptr<gfx::BatchRectangle>> tabBoxes;
};

} // namespace defcoms
} // namespace gui
} // namespace bl

#endif
