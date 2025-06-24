#ifndef BLIB_RENDER_RENDERER_SCENEBASE_HPP
#define BLIB_RENDER_RENDERER_SCENEBASE_HPP

#include <BLIB/Cameras/Camera.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactoryCache.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Descriptors/ShaderInputStore.hpp>
#include <BLIB/Render/Scenes/Key.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <BLIB/Render/Vulkan/PipelineInstance.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <BLIB/Vulkan.hpp>
#include <array>
#include <mutex>
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
namespace scene
{
class SceneSync;
}

namespace res
{
class ScenePool;
}

namespace rg
{
class RenderGraph;
class Strategy;
} // namespace rg

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
     * @brief Calls renderOpaqueObjects then renderTransparentObjects
     *
     * @param context Render context containing scene render data
     */
    void renderScene(scene::SceneRenderContext& context);

    /**
     * @brief Derived classes should draw opaque objects here
     *
     * @param context Render context containing scene render data
     */
    virtual void renderOpaqueObjects(scene::SceneRenderContext& context) = 0;

    /**
     * @brief Derived classes should draw transparent objects here
     *
     * @param context Render context containing scene render data
     */
    virtual void renderTransparentObjects(scene::SceneRenderContext& context) = 0;

    /**
     * @brief Returns the render strategy to use for this scene type
     */
    virtual rg::Strategy* getRenderStrategy() = 0;

    /**
     * @brief Provides direct access to the descriptor set of the given type. Creates it if not
     *        already created. May be slow, cache the result
     *
     * @tparam T The descriptor set type to fetch or create
     * @return The descriptor set of the given type
     */
    template<typename T>
    T& getDescriptorSet();

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

    /**
     * @brief Initializes the pipeline instance for the given pipeline id
     *
     * @param pipelineId The id of the pipeline to fetch and create the instance for
     * @param instance The instance to populate with pipeline and descriptor sets
     */
    void initPipelineInstance(std::uint32_t pipelineId, vk::PipelineInstance& instance);

protected:
    /**
     * @brief POD containing data for when an object needs to be re-batched
     */
    struct BatchChange {
        scene::SceneObject* changed;
        mat::MaterialPipeline* newPipeline;
        bool newTrans;
        std::uint32_t newSpecialization;
    };

    engine::Engine& engine;
    Renderer& renderer;
    std::recursive_mutex objectMutex;
    ds::DescriptorSetFactoryCache& descriptorFactories;
    ds::DescriptorSetInstanceCache descriptorSets;
    ds::ShaderInputStore shaderInputStore;

    /**
     * @brief Initializes the Scene
     *
     * @param engine The engine instance
     * @param entityCb Callback to map scene id to ECS id
     */
    Scene(engine::Engine& engine, const scene::MapKeyToEntityCb& entityCb);

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
    virtual void doBatchChange(const BatchChange& change, mat::MaterialPipeline* ogPipeline) = 0;

    /**
     * @brief Called when an object should be removed from the scene
     *
     * @param object The object to be removed
     * @param pipeline The pipeline used to render the object being removed
     */
    virtual void doObjectRemoval(scene::SceneObject* object, mat::MaterialPipeline* pipeline) = 0;

    /**
     * @brief Called at the beginning of the frame when descriptors are being updated
     */
    virtual void onDescriptorSync() {}

private:
    struct ObjectAdd {
        ecs::Entity entity;
        rcom::DrawableBase* object;
        UpdateSpeed updateFreq;

        ObjectAdd() = default;
        ObjectAdd(ecs::Entity entity, rcom::DrawableBase* object, UpdateSpeed updateFreq)
        : entity(entity)
        , object(object)
        , updateFreq(updateFreq) {}
    };

    std::uint32_t nextObserverIndex;
    std::vector<mat::MaterialPipeline*> staticPipelines;
    std::vector<mat::MaterialPipeline*> dynamicPipelines;

    std::recursive_mutex queueMutex;
    bool isClearingQueues;
    std::vector<ObjectAdd> queuedAdds;
    std::vector<scene::SceneObject*> queuedRemovals;
    std::vector<BatchChange> queuedBatchChanges;

    void addQueuedObject(ObjectAdd& object);
    void removeQueuedObject(scene::SceneObject* object);

    // called by SceneSync
    void createAndAddObject(ecs::Entity entity, rcom::DrawableBase& object, UpdateSpeed updateFreq);

    // called by DrawableBase
    void rebucketObject(rcom::DrawableBase& object);

    // called by Observer
    void handleDescriptorSync();
    void syncObjects();
    std::uint32_t registerObserver();
    void updateObserverCamera(std::uint32_t observerIndex,
                              const ds::SceneDescriptorSetInstance::ObserverInfo& info);

    friend class scene::SceneSync;
    friend class RenderTarget;
    friend class res::ScenePool;
    friend class vk::RenderTexture;
    friend struct rcom::DrawableBase;
};

template<typename T>
T& Scene::getDescriptorSet() {
    T* set = descriptorSets.getDescriptorSet<T>();
    if (set) { return *set; }

    ds::DescriptorSetFactory* factory = descriptorFactories.getFactoryThatMakes<T>();
    if (!factory) { throw std::runtime_error("Failed to find descriptor set"); }

    return static_cast<T&>(*descriptorSets.getDescriptorSet(factory));
}

} // namespace rc
} // namespace bl

#endif
