#ifndef BLIB_RENDER_OBSERVER_HPP
#define BLIB_RENDER_OBSERVER_HPP

#include <glad/vulkan.h>

namespace bl
{
namespace render
{
class Observer {
public:
    //

private:
    VkRect2D scissor; // refreshed on window resize and observer add/remove
};

} // namespace render
} // namespace bl

#endif
