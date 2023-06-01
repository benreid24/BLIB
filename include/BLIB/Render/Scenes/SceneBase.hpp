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
class SceneBase {
protected:
    SceneBase(Renderer& renderer, std::uint32_t maxStatic, std::uint32_t maxDynamic);

    virtual ~SceneBase();

    virtual void renderScene(SceneRenderContext& context) = 0;

    virtual SceneObject* doAdd(ecs::Entity entity, std::uint32_t sceneId,
                               const prim::DrawParameters& drawParams,
                               const StagePipelines& pipelines) = 0;

    virtual void doRemove(ecs::Entity entity, SceneObject* object) = 0;

private:
    const std::uint32_t maxStatic;
    Renderer& renderer;
    ds::DescriptorSetFactoryCache& descriptorFactories;
    util::IdAllocator<std::uint32_t> staticIds;
    util::IdAllocator<std::uint32_t> dynamicIds;
    std::vector<ecs::Entity> entityMap;
    std::vector<StagePipelines> objectPipelines;
    ds::DescriptorSetInstanceCache descriptorSets;
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
    friend class Observer;
};

} // namespace scene
} // namespace render
} // namespace bl

#endif
