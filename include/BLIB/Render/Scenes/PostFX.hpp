#ifndef BLIB_RENDER_RENDERER_POSTFX_HPP
#define BLIB_RENDER_RENDERER_POSTFX_HPP

#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentSet.hpp>
#include <BLIB/Vulkan.hpp>
#include <array>

namespace bl
{
namespace gfx
{
class Observer;
class Renderer;

namespace scene
{
/**
 * @brief Base class for post-processing of rendered scenes. This class simply copies the scene
 *        unaffected by default but a different pipeline may be specified to affect the behavior. If
 *        more descriptors or push constants are needed then derived classes may override onRender()
 *        to bind what their pipeline needs
 *
 * @ingroup Renderer
 */
class PostFX {
public:
    /**
     * @brief Creates the PostFX object
     *
     * @param renderer The main renderer
     */
    PostFX(Renderer& renderer);

    /**
     * @brief Destroys the PostFX object and releases Vulkan resources
     */
    virtual ~PostFX();

    /**
     * @brief Called each update frame. Does nothing but more advanced effects can use this to
     *        update their state
     *
     * @param dt Elapsed time in seconds
     */
    virtual void update(float dt);

    /**
     * @brief Returns the bindings that must be used for pipelines utilized by this class
     */
    static std::array<VkDescriptorSetLayoutBinding, 1> DescriptorLayoutBindings();

protected:
    /**
     * @brief Configures which pipeline to use for rendering. Vertex is what is passed in. set=0
     *        binding=0 is a combined image sampler with normalized coordinates. If other
     *        descriptors are required then onRender() may be overridden to provide them. The
     *        default pipeline is Config::PipelineIds::PostFXBase
     */
    void usePipeline(std::uint32_t pipelineId);

    /**
     * @brief Does nothing on the base class. Called prior to the draw commands being issued. Bind
     *        custom descriptors here if the pipeline requires it
     *
     * @param commandBuffer The command buffer to issue commands into
     */
    virtual void onRender(VkCommandBuffer commandBuffer);

private:
    Renderer& renderer;
    VkSampler sampler;
    vk::DescriptorPool::AllocationHandle descriptorSetAllocHandle;
    vk::PerFrame<VkDescriptorSet> descriptorSets;
    buf::IndexBuffer indexBuffer;
    vk::Pipeline* pipeline;

    void bindImages(vk::PerFrame<vk::StandardAttachmentBuffers>& sceneImages);
    void compositeScene(VkCommandBuffer commandBuffer);

    friend class bl::gfx::Observer;
};

} // namespace scene
} // namespace gfx
} // namespace bl

#endif
