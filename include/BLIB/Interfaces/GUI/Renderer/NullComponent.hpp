#ifndef BLIB_GUI_RENDERER_NULLCOMPONENT_HPP
#define BLIB_GUI_RENDERER_NULLCOMPONENT_HPP

#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>

namespace bl
{
namespace gui
{
namespace rdr
{
/**
 * @brief Empty component that noops all operations and renders nothing
 *
 * @ingroup GUI
 */
class NullComponent : public rdr::Component {
public:
    /**
     * @brief Creates a new NullComponent
     */
    NullComponent()
    : Component(HighlightState::IgnoresMouse) {}

    /**
     * @brief Destroys the component
     */
    virtual ~NullComponent() = default;

private:
    virtual void setVisible(bool) override {}
    virtual void onElementUpdated() override {}
    virtual void onRenderSettingChange() override {}
    virtual ecs::Entity getEntity() const override { return ecs::InvalidEntity; }
    virtual void doCreate(engine::World&, Renderer&) override {}
    virtual void doSceneAdd(rc::Overlay*) override {}
    virtual void doSceneRemove() override {}
    virtual void notifyUIState(UIState) override {}
    virtual void handleAcquisition() override {}
    virtual void handleMove() override {}
};

} // namespace rdr
} // namespace gui
} // namespace bl

#endif
