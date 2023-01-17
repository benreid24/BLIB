#ifndef BLIB_RENDER_VULKAN_SWAPRENDERFRAME_HPP
#define BLIB_RENDER_VULKAN_SWAPRENDERFRAME_HPP

#include <BLIB/Render/Vulkan/RenderFrame.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>

namespace bl
{
namespace render
{
class Swapchain;

/**
 * @brief Render frame that exists within the Swapchain
 * 
 * @ingroup Renderer
*/
class SwapRenderFrame : public RenderFrame {
    friend class Swapchain;
};

} // namespace render
} // namespace bl

#endif
