#include <BLIB/Render/Resources/ScenePool.hpp>

#include <BLIB/Events.hpp>
#include <BLIB/Render/Events/SceneDestroyed.hpp>

namespace bl
{
namespace rc
{
namespace res
{
ScenePool::ScenePool(engine::Engine& e)
: engine(e) {
    scenes.reserve(16);
}

void ScenePool::cleanup() {
    std::unique_lock lock(mutex);
    scenes.clear();
}

void ScenePool::destroyScene(Scene* scene) {
    std::unique_lock lock(mutex);

    bl::event::Dispatcher::dispatch<rc::event::SceneDestroyed>({scene});
    for (auto it = scenes.begin(); it != scenes.end(); ++it) {
        if (it->get() == scene) {
            scenes.erase(it);
            return;
        }
    }
}

} // namespace res
} // namespace rc
} // namespace bl
