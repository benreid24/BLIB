#include <BLIB/Render/Resources/ScenePool.hpp>

#include <BLIB/Events.hpp>
#include <BLIB/Render/Events/SceneDestroyed.hpp>

namespace bl
{
namespace render
{
ScenePool::ScenePool(Renderer& r)
: renderer(r)
, pool(MaxSceneCount)
, freeSlots(MaxSceneCount) {}

void ScenePool::cleanup() {
    std::unique_lock lock(mutex);

    for (std::uint32_t i = 0; i < MaxSceneCount; ++i) {
        if (freeSlots.isAllocated(i)) { pool[i].destroy(); }
    }
    freeSlots.releaseAll();
}

Scene* ScenePool::allocateScene(std::uint32_t maxStaticObjectCount,
                                std::uint32_t maxDynamicObjectCount) {
    std::unique_lock lock(mutex);

    if (!freeSlots.available()) {
        BL_LOG_CRITICAL << "Out of scenes in the pool!";
        throw std::runtime_error("Scene pool has no available scenes");
    }

    const std::uint32_t i = freeSlots.allocate();
    pool[i].emplace(renderer, maxStaticObjectCount, maxDynamicObjectCount);
    return &pool[i].get();
}

void ScenePool::destroyScene(Scene* scene) {
    std::unique_lock lock(mutex);

    const std::uint32_t i = scene - &pool[0].get();

#ifdef BLIB_DEBUG
    if (i >= MaxSceneCount) {
        BL_LOG_CRITICAL << "Out of bounds scene: " << i;
        throw std::runtime_error("Out of bounds scene during scene destruction");
    }
#endif

    // handle double-free to make shared scenes easier to release
    if (!freeSlots.isAllocated(i)) { return; }

    bl::event::Dispatcher::dispatch<render::event::SceneDestroyed>({scene});
    pool[i].destroy();
    freeSlots.release(i);
}

} // namespace render
} // namespace bl
