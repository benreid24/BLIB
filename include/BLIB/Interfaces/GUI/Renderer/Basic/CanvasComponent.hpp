#ifndef BLIB_GUI_RENDERER_COMPONENTS_CANVASCOMPONENT_HPP
#define BLIB_GUI_RENDERER_COMPONENTS_CANVASCOMPONENT_HPP

#include <BLIB/Graphics/Sprite.hpp>
#include <BLIB/Interfaces/GUI/Renderer/CanvasComponentBase.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>
#include <BLIB/Render/Vulkan/RenderTexture.hpp>

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
class CanvasComponent : public rdr::CanvasComponentBase {
public:
    /**
     * @brief Creates the component
     */
    CanvasComponent();

    /**
     * @brief Destroys the component
     */
    virtual ~CanvasComponent() = default;

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

    /**
     * @brief Returns the render texture the canvas is rendering to
     */
    virtual rc::vk::RenderTexture& getRenderTexture() override;

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
    engine::Engine* enginePtr;
    rc::vk::RenderTexture::Handle texture;
    gfx::Sprite img;
    sf::Vector2f pos;
};

} // namespace defcoms
} // namespace gui
} // namespace bl

#endif
