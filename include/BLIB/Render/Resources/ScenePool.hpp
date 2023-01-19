#ifndef BLIB_RENDER_RESOURCES_SCENEPOOL_HPP
#define BLIB_RENDER_RESOURCES_SCENEPOOL_HPP

#include <BLIB/Containers/ObjectWrapper.hpp>
#include <BLIB/Render/Renderer/Scene.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <array>
#include <cstdint>
#include <mutex>

namespace bl
{
namespace render
{
class Renderer;

/**
 * @brief Utility class that manages a fixed size pool of scenes that can be allocated from. Owned
 *        by the renderer
 * 
 * @ingroup Renderer
 */
class ScenePool {
public:
    /// The maximum number of scenes that may exist at once
    static constexpr std::uint32_t MaxSceneCount = 128;

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
     * @return The new scene
    */
    Scene* allocateScene();

    /**
     * @brief Destroys the given scene and returns it to the pool
     * 
     * @param scene The scene to destroy and reuse
    */
    void destroyScene(Scene* scene);

private:
    Renderer& renderer;
    std::array<container::ObjectWrapper<Scene>, MaxSceneCount> pool;
    util::IdAllocator<std::uint32_t> freeSlots;
    std::mutex mutex;
};

} // namespace render
} // namespace bl

#endif
