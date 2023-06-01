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
namespace res
{
class ScenePool;
}

class Scene {
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

    // called by Observer
    void handleDescriptorSync();
    std::uint32_t registerObserver();
    void updateObserverCamera(std::uint32_t observerIndex, const glm::mat4& projView);
    void renderScene(scene::SceneRenderContext& context);

    template<typename T>
    friend class sys::GenericDrawableSystem;
    friend class container::ObjectWrapper<Scene>;
    friend class Observer;
    friend class res::ScenePool;
};

} // namespace render
} // namespace bl

#endif
