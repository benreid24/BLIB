#ifndef BLIB_RENDER_EVENTS_GRAPHEVENTS_HPP
#define BLIB_RENDER_EVENTS_GRAPHEVENTS_HPP

namespace bl
{
namespace rc
{
class RenderTarget;
class Scene;

namespace rg
{
struct GraphAsset;
}

namespace event
{
/**
 * @brief Fired when a new asset is created for a scene render graph. The asset will not have been
 *        initialized at this point
 *
 * @ingroup Renderer
 */
struct SceneGraphAssetCreated {
    RenderTarget* target;
    Scene* scene;
    rg::GraphAsset* asset;
};

/**
 * @brief Fired when an asset in a graph timeline is initialized. This means the asset is in use and
 *        resources have been created
 *
 * @ingroup Renderer
 */
struct SceneGraphAssetInitialized {
    RenderTarget* target;
    Scene* scene;
    rg::GraphAsset* asset;
};

} // namespace event
} // namespace rc
} // namespace bl

#endif
