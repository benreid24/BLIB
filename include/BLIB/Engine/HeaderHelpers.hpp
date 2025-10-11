#ifndef BLIB_RENDER_ENGINEHELPERS_HPP
#define BLIB_RENDER_ENGINEHELPERS_HPP

#include <BLIB/Signals/Channel.hpp>

namespace bl
{
namespace ecs
{
class Registry;
}

namespace rc
{
class Renderer;

namespace vk
{
struct VulkanState;
}
} // namespace rc

namespace engine
{
class Engine;

/**
 * @brief Collection of helpers to fetch modules from the engine class without needing to include
 *        Engine.hpp. Useful for header files
 *
 * @ingroup Engine
 */
struct HeaderHelpers {
    /**
     * @brief Returns the engine renderer instance
     *
     * @param engine The main engine instance
     * @return The engine renderer instance
     */
    static rc::Renderer& getRenderer(engine::Engine& engine);

    /**
     * @brief Returns the Vulkan state of the engine renderer
     *
     * @param engine The main engine instance
     * @return The Vulkan state of the engine renderer
     */
    static rc::vk::VulkanState& getVulkanState(engine::Engine& engine);

    /**
     * @brief Returns the engine ECS registry
     *
     * @param engine The main engine instance
     * @return The ECS registry of the engine
     */
    static ecs::Registry& getRegistry(engine::Engine& engine);

    /**
     * @brief Returns the signal channel for the engine
     *
     * @param engine The main engine instance
     * @return The signal channel of the engine
     */
    static sig::Channel& getEngineSignalChannel(engine::Engine& engine);

    /**
     * @brief Returns the signal channel for the renderer
     *
     * @param engine The main engine instance
     * @return The signal channel of the renderer
     */
    static sig::Channel& getRendererSignalChannel(engine::Engine& engine);
};

} // namespace engine
} // namespace bl

#endif
