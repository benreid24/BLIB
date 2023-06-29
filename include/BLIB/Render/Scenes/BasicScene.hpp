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
     * @param object The ECS component being added
     * @param sceneId The id of the new object in this scene
     * @param updateFreq Whether the object is static or dynamic
     * @return A pointer to the new scene object
     */
    virtual scene::SceneObject* doAdd(ecs::Entity entity, com::DrawableBase& object,
                                      std::uint32_t sceneId, UpdateSpeed updateFreq) override;

    /**
     * @brief Called when an object is removed from the scene. Unlink from descriptors here
     *
     * @param entity The ECS id of the entity being removed
     * @param object The scene object being removed
     * @param pipeline The pipeline the object was being rendered with
     */
    virtual void doRemove(ecs::Entity entity, scene::SceneObject* object,
                          std::uint32_t pipeline) override;

    /**
     * @brief Called by Scene in handleDescriptorSync for objects that need to be re-batched
     *
     * @param change Details of the change
     * @param ogPipeline The original pipeline of the object being changed
     */
    virtual void doBatchChange(const BatchChange& change, std::uint32_t ogPipeline) override;

private:
    struct PipelineBatch {
        PipelineBatch(vk::Pipeline& pipeline, std::uint32_t maxObjects)
        : pipeline(pipeline) {
            objects.reserve(maxObjects / 2);
        }

        vk::Pipeline& pipeline;
        std::vector<SceneObject*> objects;
    };

    struct LayoutBatch {
        LayoutBatch(ds::DescriptorSetInstanceCache& descriptorCache,
                    const vk::PipelineLayout& layout)
        : layout(layout)
        , descriptorCount(layout.initDescriptorSets(descriptorCache, descriptors.data()))
        , perObjStart(descriptorCount)
        , bindless(true) {
            batches.reserve(8);
            for (std::uint8_t i = 0; i < descriptorCount; ++i) {
                if (!descriptors[i]->isBindless()) {
                    perObjStart = i;
                    bindless    = false;
                    break;
                }
            }
        }

        const vk::PipelineLayout& layout;
        std::array<ds::DescriptorSetInstance*, Config::MaxDescriptorSets> descriptors;
        std::uint8_t descriptorCount;
        std::uint8_t perObjStart;
        std::vector<PipelineBatch> batches;
        bool bindless;
    };

    struct ObjectBatch {
        ObjectBatch() { batches.reserve(8); }
        std::vector<LayoutBatch> batches;
    };

    std::vector<scene::SceneObject> objects;
    ObjectBatch opaqueObjects;
    ObjectBatch transparentObjects;
    std::vector<bool> transCache;

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
