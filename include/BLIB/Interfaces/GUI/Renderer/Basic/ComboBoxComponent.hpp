#ifndef BLIB_GUI_RENDERER_COMPONENTS_COMBOBOXCOMPONENT_HPP
#define BLIB_GUI_RENDERER_COMPONENTS_COMBOBOXCOMPONENT_HPP

#include <BLIB/Graphics/BatchRectangle.hpp>
#include <BLIB/Graphics/BatchedShapes2D.hpp>
#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Graphics/Text.hpp>
#include <BLIB/Graphics/Triangle.hpp>
#include <BLIB/Interfaces/GUI/RenderSettings.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>
#include <list>

namespace bl
{
namespace gui
{
namespace defcoms
{
/**
 * @brief Default component type for ComboBox elements
 *
 * @ingroup GUI
 */
class ComboBoxComponent : public rdr::Component {
public:
    /**
     * @brief Creates the component
     */
    ComboBoxComponent();

    /**
     * @brief Destroys the component
     */
    virtual ~ComboBoxComponent() = default;

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
     * @brief Returns the space required to render the largest text option
     */
    virtual sf::Vector2f getRequisition() const override;

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
    struct Option {
        gfx::BatchRectangle background;
        gfx::Text text;
        bool created;

        Option()
        : created(false) {}
    };

    engine::World* worldPtr;
    rc::Overlay* currentOverlay;
    gfx::Rectangle box;
    gfx::Rectangle arrowBox;
    gfx::Triangle arrow;

    // closed elements
    gfx::Text selectedOption;

    // open elements
    gfx::Rectangle openBackground;
    gfx::BatchedShapes2D openOptionBoxes;
    std::list<Option> openOptions;

    void configureText(gfx::Text& text, const RenderSettings& settings);
    void updateOptions();
    bool optionsOutdated() const;
    void positionSelectedText();
    void highlightMoused();
};

} // namespace defcoms
} // namespace gui
} // namespace bl

#endif
