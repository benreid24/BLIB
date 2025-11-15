#ifndef BLIB_RENDER_SHADERRESOURCES_SHADOWMAPSHADERRESOURCE_HPP
#define BLIB_RENDER_SHADERRESOURCES_SHADOWMAPSHADERRESOURCE_HPP

#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Events/SettingsChanged.hpp>
#include <BLIB/Render/Events/ShadowMapsInvalidated.hpp>
#include <BLIB/Render/ShaderResources/Key.hpp>
#include <BLIB/Render/ShaderResources/ShaderResource.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/Image.hpp>
#include <BLIB/Signals/Emitter.hpp>
#include <BLIB/Signals/Listener.hpp>
#include <array>
#include <optional>

namespace bl
{
namespace rc
{
namespace sri
{
/**
 * @brief Shader resource containing the shadow map images and framebuffers
 *
 * @ingroup Renderer
 */
class ShadowMapShaderResource
: public sr::ShaderResource
, public sig::Listener<event::SettingsChanged> {
public:
    /**
     * @brief Creates the shader resource
     */
    ShadowMapShaderResource() = default;

    /**
     * @brief Destroys the shader resource
     */
    virtual ~ShadowMapShaderResource() = default;

    /**
     * @brief Returns the image of the spot shadow map at the given index
     *
     * @param index The image index to fetch
     * @return The image of the spot shadow map at the given index
     */
    const vk::Image& getSpotShadowImage(std::uint32_t index) const {
        return spotShadowMaps[index].image;
    }

    /**
     * @brief Returns the framebuffer of the spot shadow map at the given index
     *
     * @param index The framebuffer index to fetch
     * @return The framebuffer of the spot shadow map at the given index
     */
    vk::Framebuffer& getSpotShadowFramebuffer(std::uint32_t index) {
        return spotShadowMaps[index].framebuffer;
    }

    /**
     * @brief Returns the image of the point shadow map at the given index
     *
     * @param index The image index to fetch
     * @return The image of the point shadow map at the given index
     */
    const vk::Image& getPointShadowImage(std::uint32_t index) const {
        return pointShadowMaps[index].image;
    }

    /**
     * @brief Returns the framebuffer of the point shadow map at the given index
     *
     * @param index The framebuffer index to fetch
     * @return The framebuffer of the point shadow map at the given index
     */
    vk::Framebuffer& getPointShadowFramebuffer(std::uint32_t index) {
        return pointShadowMaps[index].framebuffer;
    }

private:
    struct ShadowMap {
        vk::Image image;
        std::optional<vk::AttachmentSet> attachmentSet;
        vk::Framebuffer framebuffer;
    };

    Renderer* renderer;
    std::array<ShadowMap, cfg::Limits::MaxSpotShadows> spotShadowMaps;
    std::array<ShadowMap, cfg::Limits::MaxPointShadows> pointShadowMaps;
    sig::Emitter<event::ShadowMapsInvalidated> emitter;
    unsigned int dirtyFrames;

    virtual void init(engine::Engine& engine, RenderTarget& owner) override;
    virtual void cleanup() override;
    virtual void performTransfer() override;
    virtual void copyFromSource() override;
    virtual bool dynamicDescriptorUpdateRequired() const override;
    virtual bool staticDescriptorUpdateRequired() const override;
    virtual void process(const event::SettingsChanged& e) override;
    void createImages();
};

/**
 * @brief The shader resource key for accessing shadow maps
 *
 * @ingroup Renderer
 */
constexpr sr::Key<ShadowMapShaderResource> ShadowMapResourceKey{"__builtin_ShadowMaps"};

} // namespace sri
} // namespace rc
} // namespace bl

#endif
