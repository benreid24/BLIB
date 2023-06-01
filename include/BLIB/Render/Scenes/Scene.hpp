#ifndef BLIB_RENDER_RENDERER_SCENE_HPP
#define BLIB_RENDER_RENDERER_SCENE_HPP

#include <BLIB/Render/Scenes/SceneBase.hpp>

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

/**
 * @brief Primary scene class for the renderer. Provides batched rendering of objects by pipeline.
 *        Renders transparent objects after rendering all opaque objects
 *
 * @ingroup Renderer
 */
class Scene : public scene::SceneBase {
protected:
    /**
     * @brief Initializes the Scene
     *
     * @param renderer The renderer instance
     * @param maxStatic The maximum number of static objects in the scene
     * @param maxDynamic The maximum number of dynamic objects in the scene
     */
    Scene(Renderer& renderer, std::uint32_t maxStatic, std::uint32_t maxDynamic);

    /**
     * @brief Unlinks allocated objects from ECS descriptor linkages
     */
    virtual ~Scene() = default;

    /**
     * @brief Derived classes should record render commands in here
     *
     * @param context Context containing scene render data
     */
    virtual void renderScene(scene::SceneRenderContext& context) override;

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
    virtual scene::SceneObject* doAdd(ecs::Entity entity, std::uint32_t sceneId,
                                      UpdateSpeed updateFreq,
                                      const scene::StagePipelines& pipelines) override;

    /**
     * @brief Called when an object is removed from the scene. Unlink from descriptors here
     * @param entity
     * @param object
     * @param pipelines
     */
    virtual void doRemove(ecs::Entity entity, scene::SceneObject* object,
                          const scene::StagePipelines& pipelines) override;

private:
    std::vector<scene::SceneObject> objects;
    scene::StageBatch opaqueObjects;
    scene::StageBatch transparentObjects;

    template<typename T>
    friend class sys::GenericDrawableSystem;
    friend class container::ObjectWrapper<Scene>;
    friend class Observer;
    friend class res::ScenePool;
};

} // namespace render
} // namespace bl

#endif
