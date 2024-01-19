#include <BLIB/Render/Resources/ScenePool.hpp>

#include <BLIB/Engine.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Render/Events/SceneDestroyed.hpp>

namespace bl
{
namespace rc
{
namespace res
{
ScenePool::ScenePool(engine::Engine& e)
: engine(e) {}

void ScenePool::cleanup() {
    std::unique_lock lock(mutex);
    scenes.clear();
}

void ScenePool::release(Entry* entry) {
    vkDeviceWaitIdle(engine.renderer().vulkanState().device);
    std::unique_lock lock(mutex);

    bl::event::Dispatcher::dispatch<rc::event::SceneDestroyed>({entry->scene.get()});
    for (auto it = scenes.begin(); it != scenes.end(); ++it) {
        if (&(*it) == entry) {
            scenes.erase(it);
            return;
        }
    }
}

} // namespace res
} // namespace rc
} // namespace bl
