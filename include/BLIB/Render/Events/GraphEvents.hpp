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
 * @brief Fired when a new asset is created for a scene render graph
 *
 * @ingroup Renderer
 */
struct SceneGraphAssetCreated {
    RenderTarget* target;
    Scene* scene;
    rg::GraphAsset* asset;
};

} // namespace event
} // namespace rc
} // namespace bl

#endif
