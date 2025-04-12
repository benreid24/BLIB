#ifndef BLIB_RENDER_GRAPH_ASSETS_SCENEASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_SCENEASSET_HPP

#include <BLIB/Render/Graph/Asset.hpp>

namespace bl
{
namespace rc
{
class Scene;

namespace rgi
{
/**
 * @brief Basic asset that provides the scene to tasks
 *
 * @ingroup Renderer
 */
class SceneAsset : public rg::Asset {
public:
    /**
     * @brief Creates the asset from a scene
     *
     * @param scene The scene to provide
     * @param tag The tag to create the asset with
     */
    SceneAsset(Scene* scene, std::string_view tag);

    /**
     * @brief Destroys the asset
     */
    virtual ~SceneAsset() = default;

    Scene* scene;

private:
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
