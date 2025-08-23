#ifndef BLIB_RENDER_EVENTS_RENDERPASSINVALIDATED_HPP
#define BLIB_RENDER_EVENTS_RENDERPASSINVALIDATED_HPP

namespace bl
{
namespace rc
{
namespace vk
{
class RenderPass;
}
namespace event
{
/**
 * @brief Fired when a render pass is recreated for any reason
 *
 * @ingroup Renderer
 */
struct RenderPassInvalidated {
    vk::RenderPass& renderPass;
};

} // namespace event
} // namespace rc
} // namespace bl

#endif
