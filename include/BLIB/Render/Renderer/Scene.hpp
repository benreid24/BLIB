#ifndef BLIB_RENDER_RENDERER_SCENE_HPP
#define BLIB_RENDER_RENDERER_SCENE_HPP

#include <BLIB/Containers/ObjectPool.hpp>
#include <BLIB/Render/Renderer/Object.hpp>
#include <BLIB/Render/Renderer/RenderPassBatch.hpp>
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
    Scene(Renderer& renderer);

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

    void recordRenderCommands();

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
    void performRemovals();
};

} // namespace render
} // namespace bl

#endif
