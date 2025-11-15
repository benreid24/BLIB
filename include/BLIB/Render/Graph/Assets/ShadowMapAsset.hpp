#ifndef BLIB_RENDER_GRAPH_SHADOWMAPASSET_HPP
#define BLIB_RENDER_GRAPH_SHADOWMAPASSET_HPP

#include <BLIB/Render/Graph/Asset.hpp>
#include <BLIB/Render/ShaderResources/ShadowMapShaderResource.hpp>

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
        return maps->getSpotShadowImage(index);
    }

    /**
     * @brief Returns the framebuffer of the spot shadow map at the given index
     *
     * @param index The framebuffer index to fetch
     * @return The framebuffer of the spot shadow map at the given index
     */
    vk::Framebuffer& getSpotShadowFramebuffer(std::uint32_t index) {
        return maps->getSpotShadowFramebuffer(index);
    }

    /**
     * @brief Returns the image of the point shadow map at the given index
     *
     * @param index The image index to fetch
     * @return The image of the point shadow map at the given index
     */
    const vk::Image& getPointShadowImage(std::uint32_t index) const {
        return maps->getPointShadowImage(index);
    }

    /**
     * @brief Returns the framebuffer of the point shadow map at the given index
     *
     * @param index The framebuffer index to fetch
     * @return The framebuffer of the point shadow map at the given index
     */
    vk::Framebuffer& getPointShadowFramebuffer(std::uint32_t index) {
        return maps->getPointShadowFramebuffer(index);
    }

private:
    sri::ShadowMapShaderResource* maps;

    virtual void doCreate(const rg::InitContext& ctx) override;
    virtual void doPrepareForInput(const rg::ExecutionContext& context) override;
    virtual void doStartOutput(const rg::ExecutionContext& context) override;
    virtual void doEndOutput(const rg::ExecutionContext& context) override;
    void createImages();
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
