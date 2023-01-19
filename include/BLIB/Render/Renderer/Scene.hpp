#ifndef BLIB_RENDER_RENDERER_SCENE_HPP
#define BLIB_RENDER_RENDERER_SCENE_HPP

#include <BLIB/Containers/ObjectPool.hpp>
#include <BLIB/Render/Renderer/Object.hpp>
#include <BLIB/Render/Renderer/RenderPassBatch.hpp>
#include <BLIB/Render/Vulkan/RenderFrame.hpp>
#include <array>
#include <glad/vulkan.h>
#include <mutex>
#include <vector>

namespace bl
{
namespace render
{
class Renderer;
class ScenePool;

class Scene {
public:
    

    /**
     * @brief Creates a new object to be rendered in the scene
     *
     * @param owner The Renderable which will own the Object
     * @return Object* The newly created object
     */
    Object::Handle createAndAddObject(Renderable* owner);

    /**
     * @brief Removes the given object from the scene
     *
     * @param object The object to remove
     */
    void removeObject(const Object::Handle& object);

    /**
     * @brief Records the commands to render this scene into the given command buffer
     * 
     * @param target The target to render to
     * @param commandBuffer The command buffer to record commands into
    */
    void recordRenderCommands(const RenderFrame& target, VkCommandBuffer commandBuffer);

private:
    std::mutex mutex;
    Renderer& renderer;
    container::ObjectPool<Object> objects;
    std::array<RenderPassBatch*, Config::RenderPassIds::Count> renderPasses;
    RenderPassBatch shadowPass;
    RenderPassBatch opaquePass;
    RenderPassBatch transparencyPass;
    RenderPassBatch postFxPass;
    RenderPassBatch overlayPass;
    std::mutex eraseMutex;
    std::vector<Object::Handle> toRemove;

    Scene(Renderer& renderer);
    void performRemovals();
    void updatePassMembership(Object::Handle& object);

    friend class ScenePool;
};

} // namespace render
} // namespace bl

#endif
