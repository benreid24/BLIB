#ifndef BLIB_RENDER_GRAPH_EXECUTIONCONTEXT_HPP
#define BLIB_RENDER_GRAPH_EXECUTIONCONTEXT_HPP

#include <BLIB/Vulkan.hpp>
#include <cstdint>

namespace bl
{
namespace engine
{
class Engine;
}

namespace rc
{
class Renderer;

namespace rg
{
/**
 * @brief Context containing graph execution data that is passed down to tasks
 *
 * @ingroup Renderer
 */
struct ExecutionContext {
    engine::Engine& engine;
    Renderer& renderer;
    VkCommandBuffer commandBuffer;
    std::uint32_t observerIndex;
    bool renderingToRenderTexture;
    bool isFinalStep;

    /**
     * @brief Creates an execution context
     *
     * @param engine The engine instance
     * @param renderer The renderer instance
     * @param commandBuffer The command buffer to record to
     * @param observerIndex The index of the active observer
     * @param renderTexture Whether or not the final target is a render texture
     * @param isFinalStep Whether or not the execution is for the final task
     */
    ExecutionContext(engine::Engine& engine, Renderer& renderer, VkCommandBuffer commandBuffer,
                     std::uint32_t observerIndex, bool renderTexture, bool isFinalStep)
    : engine(engine)
    , renderer(renderer)
    , commandBuffer(commandBuffer)
    , observerIndex(observerIndex)
    , renderingToRenderTexture(renderTexture)
    , isFinalStep(isFinalStep) {}
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
