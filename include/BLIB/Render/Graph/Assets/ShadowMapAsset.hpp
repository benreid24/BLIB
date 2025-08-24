#ifndef BLIB_RENDER_GRAPH_SHADOWMAPASSET_HPP
#define BLIB_RENDER_GRAPH_SHADOWMAPASSET_HPP

#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Events/SettingsChanged.hpp>
#include <BLIB/Render/Events/ShadowMapsInvalidated.hpp>
#include <BLIB/Render/Graph/Asset.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Signals/Emitter.hpp>
#include <BLIB/Signals/Listener.hpp>
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
class ShadowMapAsset
: public rg::Asset
, public sig::Listener<event::SettingsChanged> {
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

    virtual void doCreate(engine::Engine& engine, Renderer& renderer,
                          RenderTarget* observer) override;
    virtual void doPrepareForInput(const rg::ExecutionContext& context) override;
    virtual void doStartOutput(const rg::ExecutionContext& context) override;
    virtual void doEndOutput(const rg::ExecutionContext& context) override;
    virtual void process(const event::SettingsChanged& e) override;
    void createImages();
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
