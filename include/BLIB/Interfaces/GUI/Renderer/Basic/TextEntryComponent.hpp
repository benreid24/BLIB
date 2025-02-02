#ifndef BLIB_GUI_RENDERER_COMPONENTS_TEXTENTRYCOMPONENT_HPP
#define BLIB_GUI_RENDERER_COMPONENTS_TEXTENTRYCOMPONENT_HPP

#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Graphics/Text.hpp>
#include <BLIB/Interfaces/GUI/Renderer/TextEntryComponentBase.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
/**
 * @brief Default component type for TextEntry elements
 *
 * @ingroup GUI
 */
class TextEntryComponent : public rdr::TextEntryComponentBase {
public:
    /**
     * @brief Creates the component
     */
    TextEntryComponent();

    /**
     * @brief Destroys the component
     */
    virtual ~TextEntryComponent() = default;

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
     * @brief Called to determine what index the carat should move to when the text is clicked
     *
     * @param overlayPos The position in overlay coordinates
     * @return The closest text index of the given position
     */
    virtual unsigned int findCursorPosition(const sf::Vector2f& overlayPos) const override;

    /**
     * @brief Called when the carat changes position or toggles visibility
     */
    virtual void onCaratStateUpdate() override;

    /**
     * @brief Called when the carat flash timer should be reset
     */
    virtual void resetCaratFlash() override;

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
    gfx::Text text;
    gfx::Rectangle carat;

    void positionItems();
};

} // namespace defcoms
} // namespace gui
} // namespace bl

#endif
