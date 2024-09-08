#ifndef BLIB_GUI_RENDERER_NULLTOOLTIPPROVIDER_HPP
#define BLIB_GUI_RENDERER_NULLTOOLTIPPROVIDER_HPP

#include <BLIB/Interfaces/GUI/Renderer/TooltipProvider.hpp>

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
class NullTooltipProvider : public TooltipProvider {
public:
    /**
     * @brief Destroys the provider
     */
    virtual ~NullTooltipProvider() = default;

private:
    virtual void update(float) override {}
    virtual void displayTooltip(Element*, const glm::vec2&) override {}
    virtual void dismissTooltip() override {}
    virtual void doCreate(engine::World&) override {}
    virtual void doSceneAdd(rc::Overlay*) override {}
    virtual void doSceneRemove() override {}
    virtual void notifyDestroyed(const Element*) override {}
};

} // namespace rdr
} // namespace gui
} // namespace bl

#endif
