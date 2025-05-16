#ifndef BLIB_RENDER_GRAPH_SHADOWMAPASSET_HPP
#define BLIB_RENDER_GRAPH_SHADOWMAPASSET_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Graph/Asset.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <array>
#include <optional>

namespace bl
{
namespace rc
{
class Scene;

namespace rgi
{
/**
 * @brief Asset that contains framebuffers for shadow maps
 *
 * @ingroup Renderer
 */
class ShadowMapAsset : public rg::Asset {
public:
    /**
     * @brief Creates the shadow map asset
     *
     * @param tag The asset tag
     */
    ShadowMapAsset(std::string_view tag);

    /**
     * @brief Destroys the shadow map asset
     */
    virtual ~ShadowMapAsset() = default;

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
    const vk::Framebuffer& getSpotShadowFramebuffer(std::uint32_t index) const {
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
    const vk::Framebuffer& getPointShadowFramebuffer(std::uint32_t index) const {
        return pointShadowMaps[index].framebuffer;
    }

private:
    // TODO - put actual images here instead of in descriptor
    struct ShadowMap {
        vk::Image image;
        std::optional<vk::AttachmentSet> attachmentSet;
        vk::Framebuffer framebuffer;
    };

    std::array<ShadowMap, Config::MaxSpotShadows> spotShadowMaps;
    std::array<ShadowMap, Config::MaxPointShadows> pointShadowMaps;

    virtual void doCreate(engine::Engine& engine, Renderer& renderer,
                          RenderTarget* observer) override;
    virtual void doPrepareForInput(const rg::ExecutionContext& context) override;
    virtual void doStartOutput(const rg::ExecutionContext& context) override;
    virtual void doEndOutput(const rg::ExecutionContext& context) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
