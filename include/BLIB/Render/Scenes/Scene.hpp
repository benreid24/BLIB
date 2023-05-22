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
    std::uint32_t nextObserverIndex;

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

} // namespace render
} // namespace bl

#endif
