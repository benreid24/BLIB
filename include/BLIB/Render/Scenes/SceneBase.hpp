#ifndef BLIB_RENDER_RENDERER_SCENEBASE_HPP
#define BLIB_RENDER_RENDERER_SCENEBASE_HPP

#include <BLIB/Containers/ObjectPool.hpp>
#include <BLIB/Containers/ObjectWrapper.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactoryCache.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <BLIB/Render/Scenes/StageBatch.hpp>
#include <BLIB/Render/Scenes/StagePipelines.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <array>
#include <glad/vulkan.h>
#include <vector>

namespace bl
{
namespace render
{
class Renderer;
namespace sys
{
template<typename T>
class GenericDrawableSystem;
}
namespace res
{
class ScenePool;
}
namespace scene
{
/**
 * @brief Base class for all scene types and overlays. Provides common scene logic and object
 *        management. Derived classes must provide storage for SceneObject (or derived)
 *
 * @ingroup Renderer
 */
class SceneBase {
protected:
    const std::uint32_t maxStatic;
    Renderer& renderer;
    ds::DescriptorSetFactoryCache& descriptorFactories;
    ds::DescriptorSetInstanceCache descriptorSets;

    /**
     * @brief Initializes the SceneBase
     *
     * @param renderer The renderer instance
     * @param maxStatic The maximum number of static objects in the scene
     * @param maxDynamic The maximum number of dynamic objects in the scene
     */
    SceneBase(Renderer& renderer, std::uint32_t maxStatic, std::uint32_t maxDynamic);

    /**
     * @brief Unlinks allocated objects from ECS descriptor linkages
     */
    virtual ~SceneBase();

    /**
     * @brief Derived classes should record render commands in here
     *
     * @param context Context containing scene render data
     */
    virtual void renderScene(SceneRenderContext& context) = 0;

    /**
     * @brief Called when an object is added to the scene. Derived should create the SceneObject
     *        here and initialize descriptor sets
     *
     * @param entity The ECS entity of the new object
     * @param sceneId The id of the new object in this scene
     * @param updateFreq Whether the object is static or dynamic
     * @param pipelines Which pipelines to use to render the object
     * @return A pointer to the new scene object
     */
    virtual SceneObject* doAdd(ecs::Entity entity, std::uint32_t sceneId, UpdateSpeed updateFreq,
                               const StagePipelines& pipelines) = 0;

    /**
     * @brief Called when an object is removed from the scene. Unlink from descriptors here
     * @param entity
     * @param object
     * @param pipelines
     */
    virtual void doRemove(ecs::Entity entity, SceneObject* object,
                          const StagePipelines& pipelines) = 0;

private:
    util::IdAllocator<std::uint32_t> staticIds;
    util::IdAllocator<std::uint32_t> dynamicIds;
    std::vector<ecs::Entity> entityMap;
    std::vector<StagePipelines> objectPipelines;
    std::uint32_t nextObserverIndex;

    // called by sys::GenericDrawableSystem in locked context
    SceneObject* createAndAddObject(ecs::Entity entity, const prim::DrawParameters& drawParams,
                                    UpdateSpeed updateFreq, const StagePipelines& pipelines);
    void removeObject(SceneObject* object);

    // called by Observer
    void handleDescriptorSync();
    std::uint32_t registerObserver();
    void updateObserverCamera(std::uint32_t observerIndex, const glm::mat4& projView);

    template<typename T>
    friend class sys::GenericDrawableSystem;
    friend class container::ObjectWrapper<SceneBase>;
    friend class Observer;
    friend class res::ScenePool;
};

} // namespace scene
} // namespace render
} // namespace bl

#endif
