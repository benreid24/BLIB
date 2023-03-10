#ifndef BLIB_RENDER_RENDERER_SCENE_HPP
#define BLIB_RENDER_RENDERER_SCENE_HPP

#include <BLIB/Containers/ObjectPool.hpp>
#include <BLIB/Containers/ObjectWrapper.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
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
     * @brief Records the commands to render this scene into the given command buffer
     *
     * @param context Render context containing the parameters to render with
     */
    void renderScene(SceneRenderContext& context);

    // TODO - overlay method

private:
    const std::uint32_t maxStatic;
    Renderer& renderer;
    std::vector<SceneObject> objects;
    util::IdAllocator<std::uint32_t> staticIds;
    util::IdAllocator<std::uint32_t> dynamicIds;
    std::vector<ecs::Entity> entityMap;
    std::vector<scene::StagePipelines> objectPipelines;
    ds::DescriptorSetInstanceCache descriptorSets;

    StageBatch opaqueObjects;
    StageBatch transparentObjects;

    Scene(Renderer& renderer, std::uint32_t maxStatic, std::uint32_t maxDynamic);
    ~Scene();

    // called by sys::GenericDrawableSystem in locked context
    SceneObject* createAndAddObject(ecs::Entity entity, const DrawParameters& drawParams,
                                    SceneObject::UpdateSpeed updateFreq,
                                    const scene::StagePipelines& pipelines);
    void removeObject(SceneObject* object);
    void removeObject(ecs::Entity entity);

    template<typename T>
    friend class sys::GenericDrawableSystem;
    friend class container::ObjectWrapper<Scene>;
};

} // namespace render
} // namespace bl

#endif
