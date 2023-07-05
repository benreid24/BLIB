#ifndef BLIB_RENDER_RENDERER_SCENEBASE_HPP
#define BLIB_RENDER_RENDERER_SCENEBASE_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Descriptors/DescriptorComponentStorageCache.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactoryCache.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <array>
#include <glad/vulkan.h>
#include <vector>

namespace bl
{
namespace gfx
{
class Renderer;

namespace vk
{
class RenderTexture;
}

namespace sys
{
template<typename T>
class DrawableSystem;
}
namespace res
{
class ScenePool;
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
     * @brief Derived classes should record render commands in here
     *
     * @param context Render context containing scene render data
     */
    virtual void renderScene(scene::SceneRenderContext& context) = 0;

    /**
     * @brief Called when an object is added to the scene. Derived should create the SceneObject
     *        here and initialize descriptor sets
     *
     * @param entity The ECS entity of the new object
     * @param object The ECS component being added
     * @param updateFreq The update speed of the new object
     * @return A pointer to the new scene object
     */
    virtual scene::SceneObject* doAdd(ecs::Entity entity, com::DrawableBase& object,
                                      UpdateSpeed updateFreq) = 0;

    /**
     * @brief Called when an object is removed from the scene. Unlink from descriptors here
     *
     * @param object The object being removed
     * @param pipeline The pipeline used to render the object being removed
     */
    virtual void doRemove(scene::SceneObject* object, std::uint32_t pipeline) = 0;

    /**
     * @brief Called by Scene in handleDescriptorSync for objects that need to be re-batched
     *
     * @param change Details of the change
     * @param ogPipeline The original pipeline of the object being changed
     */
    virtual void doBatchChange(const BatchChange& change, std::uint32_t ogPipeline) = 0;

    /**
     * @brief Intended to be called by DrawableSystem. Can be used by derived classes to
     *        remove child objects
     *
     * @param object The object to remove
     */
    void removeObject(scene::SceneObject* object);

    /**
     * @brief Returns the ECS id for the object with the given scene id
     *
     * @param sceneId The scene id of the object to lookup
     * @return The ECS id of the entity
     */
    constexpr ecs::Entity getEntityFromId(std::uint32_t sceneId) const;

private:
    std::uint32_t nextObserverIndex;
    std::mutex batchMutex;
    std::vector<BatchChange> batchChanges;
    std::vector<std::uint32_t> staticPipelines;
    std::vector<std::uint32_t> dynamicPipelines;

    // called by sys::DrawableSystem in locked context
    void createAndAddObject(ecs::Entity entity, com::DrawableBase& object, UpdateSpeed updateFreq);

    // called by DrawableBase
    void rebucketObject(com::DrawableBase& object);

    // called by Observer
    void handleDescriptorSync();
    std::uint32_t registerObserver();
    void updateObserverCamera(std::uint32_t observerIndex, const glm::mat4& projView);

    template<typename T>
    friend class sys::DrawableSystem;
    friend class Observer;
    friend class res::ScenePool;
    friend class vk::RenderTexture;
    friend struct com::DrawableBase;
};

} // namespace gfx
} // namespace bl

#endif
