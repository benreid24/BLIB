#ifndef BLIB_RENDER_GRAPH_TASKS_SHADOWMAPTASK_HPP
#define BLIB_RENDER_GRAPH_TASKS_SHADOWMAPTASK_HPP

#include <BLIB/Render/Graph/Assets/ShadowMapAsset.hpp>
#include <BLIB/Render/Graph/Task.hpp>

namespace bl
{
namespace rc
{
namespace scene
{
class Scene3D;
}

namespace rgi
{
/**
 * @brief Task that renders shadow maps
 *
 * @ingroup Renderer
 */
class ShadowMapTask : public rg::Task {
public:
    /**
     * @brief Creates the task
     */
    ShadowMapTask();

    /**
     * @brief Destroys the task
     */
    virtual ~ShadowMapTask() = default;

private:
    Renderer* renderer;
    scene::Scene3D* scene;
    ShadowMapAsset* shadowMaps;

    virtual void create(engine::Engine& engine, Renderer& renderer, Scene* scene) override;
    virtual void onGraphInit() override;
    virtual void execute(const rg::ExecutionContext& ctx, rg::Asset* output) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
