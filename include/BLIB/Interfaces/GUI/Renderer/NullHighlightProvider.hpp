#ifndef BLIB_GUI_RENDERER_NULLHIGHLIGHTPROVIDER_HPP
#define BLIB_GUI_RENDERER_NULLHIGHLIGHTPROVIDER_HPP

#include <BLIB/Interfaces/GUI/Renderer/HighlightProvider.hpp>

namespace bl
{
namespace gui
{
namespace rdr
{
/**
 * @brief Does nothing
 *
 * @ingroup GUI
 */
class NullHighlightProvider : public HighlightProvider {
public:
    /**
     * @brief Destroys the provider
     */
    virtual ~NullHighlightProvider() = default;

private:
    virtual void update(float) override {}
    virtual void notifyUIState(Element*, Component::UIState) override {}
    virtual void doCreate(engine::World&) override {}
    virtual void doSceneAdd(rc::Overlay*) override {}
    virtual void doSceneRemove() override {}
    virtual void notifyDestroyed(const Element*) override {}
};

} // namespace rdr
} // namespace gui
} // namespace bl

#endif
