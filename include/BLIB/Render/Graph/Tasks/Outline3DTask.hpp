#ifndef BLIB_RENDER_GRAPH_TASKS_OUTLINE3DTASK_HPP
#define BLIB_RENDER_GRAPH_TASKS_OUTLINE3DTASK_HPP

#include <BLIB/Components/Outline.hpp>
#include <BLIB/Components/Rendered.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/Render/Graph/Task.hpp>
#include <BLIB/Render/Vulkan/PipelineInstance.hpp>
#include <BLIB/Signals/Listener.hpp>
#include <vector>

namespace bl
{
namespace ecs
{
class Registry;
}
namespace rc
{
namespace rgi
{
class Outline3DTask
: public rg::Task
, public sig::Listener<
      ecs::event::ComponentAdded<com::Outline>, ecs::event::ComponentAdded<com::Rendered>,
      ecs::event::ComponentRemoved<com::Outline>, ecs::event::ComponentRemoved<com::Rendered>> {
public:
    /**
     * @brief Initializes the task
     */
    Outline3DTask();

    /**
     * @brief Destroys the task
     */
    virtual ~Outline3DTask() = default;

private:
    struct Outlined {
        com::Rendered* drawable;
        com::Outline* outline;
    };

    Scene* scene;
    ecs::Registry* registry;
    std::vector<Outlined> outlinedObjectsDynamic;
    std::vector<Outlined> outlinedObjectsStatic;
    std::vector<Outlined> outlinedSkinnedObjectsDynamic;
    std::vector<Outlined> outlinedSkinnedObjectsStatic;
    vk::PipelineInstance standardPipeline;
    vk::PipelineInstance skinnedPipeline;

    void addObject(ecs::Entity entity, com::Rendered* drawable, com::Outline* outline);
    void removeObject(ecs::Entity entity);

    // Task
    virtual void create(const rg::InitContext& ctx) override;
    virtual void onGraphInit() override;
    virtual void execute(const rg::ExecutionContext& ctx, rg::Asset* output) override;
    virtual void update(float dt) override;

    // Events
    virtual void process(const ecs::event::ComponentAdded<com::Outline>& event) override;
    virtual void process(const ecs::event::ComponentAdded<com::Rendered>& event) override;
    virtual void process(const ecs::event::ComponentRemoved<com::Outline>& event) override;
    virtual void process(const ecs::event::ComponentRemoved<com::Rendered>& event) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
