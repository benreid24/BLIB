#ifndef BLIB_RENDER_RENDERER_SCENEBASE_HPP
#define BLIB_RENDER_RENDERER_SCENEBASE_HPP

#include <BLIB/Cameras/Camera.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Descriptors/DescriptorComponentStorageCache.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactoryCache.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <BLIB/Vulkan.hpp>
#include <array>
#include <vector>

namespace bl
{
namespace sys
{
template<typename T>
class DrawableSystem;
}

namespace rc
{
class Renderer;

namespace vk
{
class RenderTexture;
}

namespace res
{
class ScenePool;
}

namespace rg
{
class RenderGraph;
}

/**
 * @brief Base class for all scene types and overlays. Provides common scene logic and object
 *        management. Derived classes must provide storage for SceneObject (or derived)
 *
 * @ingroup Renderer
 */
class Scene {
public:
    static constexpr std::uint32_t DefaultSceneObjectCapacity = 128;

    /**
     * @brief Destroys the Scene
     */
    virtual ~Scene() = default;

    /**
     * @brief Derived classes should record render commands in here
     *
     * @param context Render context containing scene render data
     */
    virtual void renderScene(scene::SceneRenderContext& context) = 0;

    /**
     * @brief Adds scene specific tasks to the render graph. Default adds nothing
     *
     * @param graph The graph to populate
     */
    virtual void addGraphTasks(rg::RenderGraph& graph);

    /**
     * @brief Creates a default camera for the scene
     *
     * @return The default camera to use
     */
    virtual std::unique_ptr<cam::Camera> createDefaultCamera() = 0;

    /**
     * @brief Called once when a new camera is created by someone other than the scene. Not called
     *        for cameras created in createDefaultCamera()
     *
     * @param camera The camera to initialize
     */
    virtual void setDefaultNearAndFarPlanes(cam::Camera& camera) const = 0;

protected:
    /**
     * @brief POD containing data for when an object needs to be re-batched
     */
    struct BatchChange {
        scene::SceneObject* changed;
        std::uint32_t newPipeline;
        bool newTrans;
    };

    Renderer& renderer;
    ds::DescriptorSetFactoryCache& descriptorFactories;
    ds::DescriptorSetInstanceCache descriptorSets;
    ds::DescriptorComponentStorageCache descriptorComponents;

    /**
     * @brief Initializes the Scene
     *
     * @param engine The engine instance
     * @param entityCb Callback to map scene id to ECS id
     */
    Scene(engine::Engine& engine,
          const ds::DescriptorComponentStorageBase::EntityCallback& entityCb);

    /**
     * @brief Called when an object is added to the scene. Derived should create the SceneObject
     *        here and initialize descriptor sets
     *
     * @param entity The ECS entity of the new object
     * @param object The ECS component being added
     * @param updateFreq The update speed of the new object
     * @return A pointer to the new scene object
     */
    virtual scene::SceneObject* doAdd(ecs::Entity entity, rcom::DrawableBase& object,
                                      UpdateSpeed updateFreq) = 0;

    /**
     * @brief Call to remove an object from the scene. Removal is queued
     *
     * @param object The object being removed
     */
    void removeObject(scene::SceneObject* object);

    /**
     * @brief Called by Scene in handleDescriptorSync for objects that need to be re-batched
     *
     * @param change Details of the change
     * @param ogPipeline The original pipeline of the object being changed
     */
    virtual void doBatchChange(const BatchChange& change, std::uint32_t ogPipeline) = 0;

    /**
     * @brief Called when an object should be removed from the scene. Derived scenes should maintain
     *        a queue of objects marked for removal. If necessary, derived scenes should make copies
     *        of objects to be removed in case it is possible that the underlying memory is freed
     *        before the queue is drained
     *
     * @param object The object to be removed
     * @param pipeline The pipeline used to render the object being removed
     */
    virtual void queueObjectRemoval(scene::SceneObject* object, std::uint32_t pipeline) = 0;

    /**
     * @brief Called once per frame. Derived classes should remove all queued objects here
     */
    virtual void removeQueuedObjects() = 0;

private:
    std::uint32_t nextObserverIndex;
    std::mutex batchMutex;
    std::vector<BatchChange> batchChanges;
    std::vector<std::uint32_t> staticPipelines;
    std::vector<std::uint32_t> dynamicPipelines;

    // called by sys::DrawableSystem in locked context
    void createAndAddObject(ecs::Entity entity, rcom::DrawableBase& object, UpdateSpeed updateFreq);

    // called by DrawableBase
    void rebucketObject(rcom::DrawableBase& object);

    // called by Observer
    void handleDescriptorSync();
    std::uint32_t registerObserver();
    void updateObserverCamera(std::uint32_t observerIndex, const glm::mat4& projView);
    // TODO - virtual hook for updateCam to allow derived scenes to prepare descriptors (ie lights)

    template<typename T>
    friend class sys::DrawableSystem;
    friend class Observer;
    friend class res::ScenePool;
    friend class vk::RenderTexture;
    friend struct rcom::DrawableBase;
};

} // namespace rc
} // namespace bl

#endif
