#ifndef BLIB_RENDER_VULKAN_RENDERPASS_HPP
#define BLIB_RENDER_VULKAN_RENDERPASS_HPP

#include <BLIB/Render/Events/RenderPassInvalidated.hpp>
#include <BLIB/Render/Events/SettingsChanged.hpp>
#include <BLIB/Render/Events/TextureFormatChanged.hpp>
#include <BLIB/Render/Vulkan/RenderPassParameters.hpp>
#include <BLIB/Render/Vulkan/VulkanLayer.hpp>
#include <BLIB/Signals/Emitter.hpp>
#include <BLIB/Signals/Listener.hpp>

namespace bl
{
namespace rc
{
class Renderer;
namespace vk
{
/**
 * @brief Represents a render pass in the renderer. A scene is comprised of a sequence of render
 *        passes and each render pass is comprised of a set of Pipelines and their renderables
 *
 * @ingroup Renderer
 */
class RenderPass : public sig::Listener<event::SettingsChanged, event::TextureFormatChanged> {
public:
    /**
     * @brief Construct a new Render Pass
     *
     * @param id The id of the render pass
     * @param vulkanState The renderer Vulkan state
     * @param params The parameters to create the pass with
     */
    RenderPass(std::uint32_t id, Renderer& renderer, RenderPassParameters&& params);

    /**
     * @brief Destroy the Render Pass object
     *
     */
    ~RenderPass();

    /**
     * @brief Returns the Vulkan render pass handle
     */
    VkRenderPass rawPass() const;

    /**
     * @brief Returns the id of the render pass
     */
    std::uint32_t getId() const;

    /**
     * @brief Returns the parameters the pass was created with
     */
    const RenderPassParameters& getCreateParams() const { return createParams; }

private:
    const std::uint32_t id;
    Renderer& renderer;
    RenderPassParameters createParams;
    VkRenderPass renderPass;
    sig::Emitter<event::RenderPassInvalidated> emitter;

    virtual void process(const event::SettingsChanged& e) override;
    virtual void process(const event::TextureFormatChanged& e) override;
    void recreate();
    void doCreate();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline VkRenderPass RenderPass::rawPass() const { return renderPass; }

inline std::uint32_t RenderPass::getId() const { return id; }

} // namespace vk
} // namespace rc
} // namespace bl

#endif
