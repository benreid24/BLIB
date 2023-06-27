#ifndef BLIB_RENDER_RENDERER_SCENEBASE_HPP
#define BLIB_RENDER_RENDERER_SCENEBASE_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactoryCache.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <BLIB/Render/Scenes/StageBatch.hpp>
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
    /**
     * @brief Unlinks allocated objects from ECS descriptor linkages
     */
    virtual ~Scene();

protected:
    const std::uint32_t maxStatic;
    Renderer& renderer;
    ds::DescriptorSetFactoryCache& descriptorFactories;
    ds::DescriptorSetInstanceCache descriptorSets;

    /**
     * @brief Initializes the Scene
     *
     * @param renderer The renderer instance
     * @param maxStatic The maximum number of static objects in the scene
     * @param maxDynamic The maximum number of dynamic objects in the scene
     */
    Scene(Renderer& renderer, std::uint32_t maxStatic, std::uint32_t maxDynamic);

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
     * @param sceneId The id of the new object in this scene
     * @param updateFreq Whether the object is static or dynamic
     * @param pipeline The pipeline to use to render the object
     * @return A pointer to the new scene object
     */
    virtual scene::SceneObject* doAdd(ecs::Entity entity, std::uint32_t sceneId,
                                      UpdateSpeed updateFreq, std::uint32_t pipeline) = 0;

    /**
     * @brief Called when an object is removed from the scene. Unlink from descriptors here
     *
     * @param entity The ECS entity being removed
     * @param object The object being removed
     * @param pipeline The pipeline used to render the object being removed
     */
    virtual void doRemove(ecs::Entity entity, scene::SceneObject* object,
                          std::uint32_t pipeline) = 0;

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
    util::IdAllocator<std::uint32_t> staticIds;
    util::IdAllocator<std::uint32_t> dynamicIds;
    std::vector<ecs::Entity> entityMap;
    std::vector<std::uint32_t> objectPipelines;
    std::uint32_t nextObserverIndex;

    // called by sys::DrawableSystem in locked context
    scene::SceneObject* createAndAddObject(ecs::Entity entity,
                                           const prim::DrawParameters& drawParams,
                                           UpdateSpeed updateFreq, std::uint32_t pipeline);

    // called by Observer
    void handleDescriptorSync();
    std::uint32_t registerObserver();
    void updateObserverCamera(std::uint32_t observerIndex, const glm::mat4& projView);

    template<typename T>
    friend class sys::DrawableSystem;
    friend class Observer;
    friend class res::ScenePool;
    friend class vk::RenderTexture;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr ecs::Entity Scene::getEntityFromId(std::uint32_t sceneId) const {
    return entityMap[sceneId];
}

} // namespace gfx
} // namespace bl

#endif
