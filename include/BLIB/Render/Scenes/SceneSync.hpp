#ifndef BLIB_RENDER_SCENES_SCENESYNC_HPP
#define BLIB_RENDER_SCENES_SCENESYNC_HPP

#include <BLIB/Components/Rendered.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Render/Events/SceneDestroyed.hpp>
#include <BLIB/Render/Events/SceneObjectRemoved.hpp>

namespace bl
{
namespace ecs
{
class Registry;
}

namespace rc
{
class Renderer;

namespace scene
{
/**
 * @brief An instance of this object will be owned by the Renderer to synchronize entities in the
 *        ECS to the scenes that they are added to and removed from via the Rendered component
 *
 * @ingroup Renderer
 */
class SceneSync
: public bl::event::Listener<ecs::event::ComponentAdded<com::Rendered>,
                             ecs::event::ComponentRemoved<com::Rendered>,
                             rc::event::SceneObjectRemoved, rc::event::SceneDestroyed> {
public:
    /**
     * @brief Destroys the sync object
     */
    virtual ~SceneSync() = default;

private:
    ecs::Registry& registry;

    SceneSync(ecs::Registry& registry);
    virtual void observe(const ecs::event::ComponentAdded<com::Rendered>& event) override;
    virtual void observe(const ecs::event::ComponentRemoved<com::Rendered>& event) override;
    virtual void observe(const rc::event::SceneObjectRemoved& event) override;
    virtual void observe(const rc::event::SceneDestroyed& event) override;

    friend class ::bl::rc::Renderer;
};

} // namespace scene
} // namespace rc
} // namespace bl

#endif
