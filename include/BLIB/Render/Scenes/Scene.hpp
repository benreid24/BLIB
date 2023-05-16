#ifndef BLIB_RENDER_RENDERER_SCENE_HPP
#define BLIB_RENDER_RENDERER_SCENE_HPP

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

class Scene {
public:
    /**
     * @brief Called once prior to TransferEngine kicking off
     */
    void handleDescriptorSync();

    /**
     * @brief Registers a new observer of the scene
     *
     * @return Index of the observer in this scene
     */
    std::uint32_t registerObserver();

    /**
     * @brief Updates the camera data for the given observer index
     *
     * @param observerIndex The index of the observer to update the camera for
     * @param projView Camera matrix for the given observer
     */
    void updateObserverCamera(std::uint32_t observerIndex, const glm::mat4& projView);

    /**
     * @brief Creates and uses the given descriptor set for the scene data. Scene descriptor sets
     *        must derive from SceneDescriptorSetInstance. Default is
     *        CommonSceneDescriptorSetInstance. Only one may be used per scene. Calling multiple
     *        times will replace prior instances. Objects rendered using prior scene descriptor sets
     *        will have stale camera data
     *
     * @tparam TFactory Factory type to use to create the set
     * @tparam TSet Descriptor set type. Must be returned by the factory
     * @tparam ...TArgs Argument types to factory constructor
     * @param ...args Arguments to factory constructor
     * @return Pointer to the scene descriptor set instance
     */
    template<typename TFactory, typename TSet, typename... TArgs>
    TSet* useSceneDescriptorSet(TArgs&&... args);

    /**
     * @brief Records the commands to render this scene into the given command buffer
     *
     * @param context Render context containing the parameters to render with
     */
    void renderScene(scene::SceneRenderContext& context);

    // TODO - overlay method

private:
    const std::uint32_t maxStatic;
    Renderer& renderer;
    ds::DescriptorSetFactoryCache& descriptorFactories;
    std::vector<scene::SceneObject> objects;
    util::IdAllocator<std::uint32_t> staticIds;
    util::IdAllocator<std::uint32_t> dynamicIds;
    std::vector<ecs::Entity> entityMap;
    std::vector<scene::StagePipelines> objectPipelines;
    ds::DescriptorSetInstanceCache descriptorSets;
    ds::SceneDescriptorSetInstance* sceneDescriptors;

    scene::StageBatch opaqueObjects;
    scene::StageBatch transparentObjects;

    Scene(Renderer& renderer, std::uint32_t maxStatic, std::uint32_t maxDynamic);
    ~Scene();

    // called by sys::GenericDrawableSystem in locked context
    scene::SceneObject* createAndAddObject(ecs::Entity entity,
                                           const prim::DrawParameters& drawParams,
                                           UpdateSpeed updateFreq,
                                           const scene::StagePipelines& pipelines);
    void removeObject(scene::SceneObject* object);
    void removeObject(ecs::Entity entity);

    template<typename T>
    friend class sys::GenericDrawableSystem;
    friend class container::ObjectWrapper<Scene>;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TFactory, typename TSet, typename... TArgs>
TSet* Scene::useSceneDescriptorSet(TArgs&&... args) {
    static_assert(std::is_base_of_v<ds::SceneDescriptorSetInstance, TSet>,
                  "Scene descriptor set must derive from SceneDescriptorSetInstance");

    auto* factory =
        descriptorFactories.getOrCreateFactory<TFactory, TArgs...>(std::forward<TArgs>(args)...);
    sceneDescriptors = static_cast<TSet*>(descriptorSets.getDescriptorSet(factory));
    return static_cast<TSet*>(sceneDescriptors);
}

} // namespace render
} // namespace bl

#endif
