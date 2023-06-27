#ifndef BLIB_RENDER_RENDERER_BASICSCENE_HPP
#define BLIB_RENDER_RENDERER_BASICSCENE_HPP

#include <BLIB/Render/Scenes/Scene.hpp>

namespace bl
{
namespace gfx
{
class Renderer;
namespace sys
{
template<typename T>
class DrawableSystem;
}
namespace res
{
class ScenePool;
}
namespace scene
{
/**
 * @brief Primary scene class for the renderer. Provides batched rendering of objects by pipeline.
 *        Renders transparent objects after rendering all opaque objects
 *
 * @ingroup Renderer
 */
class BasicScene : public Scene {
public:
    /**
     * @brief Initializes the BasicScene
     *
     * @param renderer The renderer instance
     * @param maxStatic The maximum number of static objects in the scene
     * @param maxDynamic The maximum number of dynamic objects in the scene
     */
    BasicScene(Renderer& renderer, std::uint32_t maxStatic, std::uint32_t maxDynamic);

    /**
     * @brief Unlinks allocated objects from ECS descriptor linkages
     */
    virtual ~BasicScene() = default;

protected:
    /**
     * @brief Derived classes should record render commands in here
     *
     * @param context Render context containing scene render data
     */
    virtual void renderScene(scene::SceneRenderContext& context) override;

    /**
     * @brief Called when an object is added to the scene. Derived should create the SceneObject
     *        here and initialize descriptor sets
     *
     * @param entity The ECS entity of the new object
     * @param sceneId The id of the new object in this scene
     * @param updateFreq Whether the object is static or dynamic
     * @param pipeline Which pipeline to use to render the object
     * @return A pointer to the new scene object
     */
    virtual scene::SceneObject* doAdd(ecs::Entity entity, std::uint32_t sceneId,
                                      UpdateSpeed updateFreq, std::uint32_t pipeline) override;

    /**
     * @brief Called when an object is removed from the scene. Unlink from descriptors here
     *
     * @param entity The ECS id of the entity being removed
     * @param object The scene object being removed
     * @param pipeline The pipeline the object was being rendered with
     */
    virtual void doRemove(ecs::Entity entity, scene::SceneObject* object,
                          std::uint32_t pipeline) override;

private:
    std::vector<scene::SceneObject> objects;
    scene::StageBatch opaqueObjects;
    scene::StageBatch transparentObjects;

    template<typename T>
    friend class sys::DrawableSystem;
    friend class container::ObjectWrapper<BasicScene>;
    friend class Observer;
    friend class res::ScenePool;
};

} // namespace scene
} // namespace gfx
} // namespace bl

#endif
