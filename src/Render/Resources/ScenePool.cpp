#include <BLIB/Render/Resources/ScenePool.hpp>

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

Scene* ScenePool::allocateScene() {
    std::unique_lock lock(mutex);

    if (!freeSlots.available()) {
        BL_LOG_CRITICAL << "Out of scenes in the pool!";
        throw std::runtime_error("Scene pool has no available scenes");
    }

    const std::uint32_t i = freeSlots.allocate();
    pool[i].emplace(renderer);
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
    if (!freeSlots.isAllocated(i)) {
        BL_LOG_CRITICAL << "Double freeing scene: " << i;
        throw std::runtime_error("Double-free scene");
    }
#endif

    pool[i].destroy();
    freeSlots.release(i);
}

} // namespace render
} // namespace bl