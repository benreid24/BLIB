#ifndef BLIB_RENDER_RENDERER_SCENE_HPP
#define BLIB_RENDER_RENDERER_SCENE_HPP

#include <BLIB/Containers/ObjectPool.hpp>
#include <BLIB/Containers/ObjectWrapper.hpp>
#include <BLIB/Render/Renderer/ObjectBatch.hpp>
#include <BLIB/Render/Renderer/SceneObject.hpp>
#include <BLIB/Render/Renderer/SceneRenderContext.hpp>
#include <BLIB/Render/Renderer/Scenes/PrimaryObjectStage.hpp>
#include <array>
#include <glad/vulkan.h>
#include <mutex>
#include <vector>

namespace bl
{
namespace render
{
class Renderer;

class Scene {
public:
    /**
     * @brief Creates a new object to be rendered in the scene
     *
     * @param owner The Renderable which will own the SceneObject
     * @return SceneObject* The newly created object
     */
    SceneObject::Handle createAndAddObject(Renderable* owner);

    /**
     * @brief Removes the given object from the scene
     *
     * @param object The object to remove
     */
    void removeObject(const SceneObject::Handle& object);

    /**
     * @brief Syncs object additions, removals, and state changes. Call once per frame
     */
    void sync();

    /**
     * @brief Records the commands to render this scene into the given command buffer
     *
     * @param context Render context containing the parameters to render with
     */
    void renderScene(const SceneRenderContext& context);

    /**
     * @brief Composites the scene with its active postfx pipeline into the swap chain image. This
     *        must be called inside of the proper render pass and with the scissor and viewport
     *        already set
     * 
     * @param commandBuffer The command buffer to record into
     */
    void compositeScene(VkCommandBuffer commandBuffer);

    // TODO - overlay method

private:
    std::mutex mutex;
    Renderer& renderer;
    container::ObjectPool<SceneObject> objects;

    scene::PrimaryObjectStage primaryObjectStage;
    std::array<ObjectBatch*, Config::SceneObjectStage::Count> stageBatches;

    // TODO - postfx stage with pipeline

    std::mutex eraseMutex;
    std::vector<SceneObject::Handle> toRemove;

    Scene(Renderer& renderer);
    void performRemovals();
    void updateStageMembership(SceneObject::Handle& object);

    friend class container::ObjectWrapper<Scene>;
};

} // namespace render
} // namespace bl

#endif
