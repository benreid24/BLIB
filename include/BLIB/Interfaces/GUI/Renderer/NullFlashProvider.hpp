#ifndef BLIB_GUI_RENDERER_NULLFLASHPROVIDER_HPP
#define BLIB_GUI_RENDERER_NULLFLASHPROVIDER_HPP

#include <BLIB/Interfaces/GUI/Renderer/FlashProvider.hpp>

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
class NullFlashProvider : public FlashProvider {
public:
    /**
     * @brief Destroys the provider
     */
    virtual ~NullFlashProvider() = default;

private:
    virtual void update(float) override {}
    virtual void flashElement(Element*) override {}
    virtual void doCreate(engine::Engine&) override {}
    virtual void doSceneAdd(rc::Overlay*) override {}
    virtual void doSceneRemove() override {}
};

} // namespace rdr
} // namespace gui
} // namespace bl

#endif
