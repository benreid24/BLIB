#ifndef BLIB_RENDER_RESOURCES_SCENEPOOL_HPP
#define BLIB_RENDER_RESOURCES_SCENEPOOL_HPP

#include <BLIB/Containers/ObjectWrapper.hpp>
#include <BLIB/Render/Scenes/BasicScene.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <array>
#include <cstdint>
#include <mutex>
#include <type_traits>

namespace bl
{
namespace render
{
class Renderer;
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
     * @param renderer The renderer that owns this pool
     */
    ScenePool(Renderer& renderer);

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
    TScene* allocateScene(TArgs&&... args);

    /**
     * @brief Destroys the given scene and returns it to the pool
     *
     * @param scene The scene to destroy and reuse
     */
    void destroyScene(Scene* scene);

private:
    Renderer& renderer;
    std::vector<std::unique_ptr<Scene>> scenes;
    std::mutex mutex;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TScene, typename... TArgs>
TScene* ScenePool::allocateScene(TArgs&&... args) {
    static_assert(std::is_base_of_v<Scene, TScene>, "TScene must derive from Scene");
    static_assert(std::is_constructible_v<TScene, Renderer&, TArgs...>,
                  "TScene constructor must accept a Renderer& as the first parameter");

    std::unique_lock lock(mutex);

    TScene* ns = new TScene(renderer, std::forward<TArgs>(args)...);
    scenes.emplace_back(ns);
    return ns;
}

} // namespace res
} // namespace render
} // namespace bl

#endif
