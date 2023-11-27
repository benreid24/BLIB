#ifndef BLIB_GUI_RENDERER_CANVASCOMPONENTBASE_HPP
#define BLIB_GUI_RENDERER_CANVASCOMPONENTBASE_HPP

#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>
#include <BLIB/Render/Vulkan/RenderTexture.hpp>

namespace bl
{
namespace gui
{
namespace rdr
{
/**
 * @brief Intermediate base class for components of Canvas elements
 *
 * @ingroup GUI
 */
class CanvasComponentBase : public Component {
public:
    /**
     * @brief Destroys the component
     */
    virtual ~CanvasComponentBase() = default;

    /**
     * @brief Returns the render texture the canvas is rendering to
     */
    virtual rc::vk::RenderTexture& getRenderTexture() = 0;

protected:
    /**
     * @brief Calls the base Component constructor
     *
     * @param highlightState The highlight state to pass to Component()
     */
    CanvasComponentBase(Component::HighlightState highlightState)
    : Component(highlightState) {}
};

} // namespace rdr
} // namespace gui
} // namespace bl

#endif
