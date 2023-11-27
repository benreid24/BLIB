#ifndef BLIB_RENDER_RESOURCES_SCENEPOOL_HPP
#define BLIB_RENDER_RESOURCES_SCENEPOOL_HPP

#include <BLIB/Containers/ObjectWrapper.hpp>
#include <BLIB/Render/Resources/SceneRef.hpp>
#include <BLIB/Render/Scenes/BatchedScene.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <atomic>
#include <cstdint>
#include <list>
#include <mutex>
#include <type_traits>

namespace bl
{
namespace sys
{
class SceneObjectRemovalSystem;
}
namespace engine
{
class Engine;
}
namespace rc
{
namespace res
{

/**
 * @brief Utility class that manages a fixed size pool of scenes that can be allocated from. Owned
 *        by the renderer
 *
 * @ingroup Renderer
 */
class ScenePool {
public:
    /**
     * @brief Creates a new scene pool
     *
     * @param engine The engine instance
     */
    ScenePool(engine::Engine& engine);

    /**
     * @brief Destroys all scenes
     */
    void cleanup();

    /**
     * @brief Allocates a new scene from the pool and returns it
     *
     * @tparam TScene The type of scene to create
     * @tparam TArgs Argument types to the scene's constructor
     * @param args Arguments to the scene's constructor
     * @return The new scene
     */
    template<typename TScene, typename... TArgs>
    SceneRef allocateScene(TArgs&&... args);

private:
    struct Entry {
        ScenePool* owner;
        std::unique_ptr<Scene> scene;
        std::atomic_uint refCount;

        Entry(ScenePool* owner, Scene* s)
        : owner(owner)
        , scene(s)
        , refCount(0) {}
    };

    engine::Engine& engine;
    std::list<Entry> scenes;
    std::mutex mutex;

    void release(Entry* entry);
    void performObjectRemovals();

    friend class bl::rc::SceneRef;
    friend class sys::SceneObjectRemovalSystem;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TScene, typename... TArgs>
SceneRef ScenePool::allocateScene(TArgs&&... args) {
    static_assert(std::is_base_of_v<Scene, TScene>, "TScene must derive from Scene");
    static_assert(std::is_constructible_v<TScene, engine::Engine&, TArgs...>,
                  "TScene constructor must accept an Engine& as the first parameter");

    std::unique_lock lock(mutex);

    auto& entry = scenes.emplace_back(this, new TScene(engine, std::forward<TArgs>(args)...));
    return SceneRef(&entry);
}

} // namespace res

inline SceneRef::SceneRef(void* handle)
: handle(handle) {
    res::ScenePool::Entry* entry = static_cast<res::ScenePool::Entry*>(handle);
    scene                        = entry->scene.get();
    incrementRefCount();
}

inline SceneRef::SceneRef(const SceneRef& copy)
: scene(copy.scene)
, handle(copy.handle) {
    incrementRefCount();
}

inline SceneRef& SceneRef::operator=(const SceneRef& c) {
    decrementRefCount();
    scene  = c.scene;
    handle = c.handle;
    incrementRefCount();
    return *this;
}

inline void SceneRef::incrementRefCount() {
    if (scene) { ++static_cast<res::ScenePool::Entry*>(handle)->refCount; }
}

inline void SceneRef::decrementRefCount() {
    if (scene) {
        res::ScenePool::Entry* entry = static_cast<res::ScenePool::Entry*>(handle);
        if (--entry->refCount == 0) { entry->owner->release(entry); }
    }
}

inline void SceneRef::release() {
    decrementRefCount();
    scene = nullptr;
}

} // namespace rc
} // namespace bl

#endif
