#ifndef BLIB_RENDER_SCENES_SCENESYNC_HPP
#define BLIB_RENDER_SCENES_SCENESYNC_HPP

#include <BLIB/Components/Rendered.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/Render/Events/SceneDestroyed.hpp>
#include <BLIB/Render/Events/SceneObjectRemoved.hpp>
#include <BLIB/Signals/Channel.hpp>
#include <BLIB/Signals/Router.hpp>

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
class SceneSync {
public:
    /**
     * @brief Destroys the sync object
     */
    virtual ~SceneSync() = default;

private:
    ecs::Registry& registry;
    sig::Router<ecs::event::ComponentAdded<com::Rendered>,
                ecs::event::ComponentRemoved<com::Rendered>>
        ecsRouter;
    sig::Router<rc::event::SceneObjectRemoved, rc::event::SceneDestroyed> renderRouter;

    SceneSync(ecs::Registry& registry);
    void handleComponentAdd(const ecs::event::ComponentAdded<com::Rendered>& event);
    void handleComponentRemove(const ecs::event::ComponentRemoved<com::Rendered>& event);
    void handleSceneRemove(const rc::event::SceneObjectRemoved& event);
    void handleSceneDestroy(const rc::event::SceneDestroyed& event);

    void subscribe(sig::Channel& renderChannel);
    void unsubscribe();

    friend class ::bl::rc::Renderer;
};

} // namespace scene
} // namespace rc
} // namespace bl

#endif
