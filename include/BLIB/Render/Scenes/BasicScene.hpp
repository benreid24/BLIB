#ifndef BLIB_RENDER_RENDERER_BASICSCENE_HPP
#define BLIB_RENDER_RENDERER_BASICSCENE_HPP

#include <BLIB/Render/Scenes/Scene.hpp>
#include <BLIB/Render/Scenes/SceneObjectStorage.hpp>

namespace bl
{
namespace rc
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
     * @param engine The engine instance
     */
    BasicScene(engine::Engine& engine);

    /**
     * @brief Unlinks allocated objects from ECS descriptor linkages
     */
    virtual ~BasicScene();

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
    virtual scene::SceneObject* doAdd(ecs::Entity entity, rcom::DrawableBase& object,
                                      UpdateSpeed updateFreq) override;

    /**
     * @brief Called when an object is removed from the scene. Unlink from descriptors here
     *
     * @param object The scene object being removed
     * @param pipeline The pipeline the object was being rendered with
     */
    virtual void doRemove(scene::SceneObject* object, std::uint32_t pipeline) override;

    /**
     * @brief Called by Scene in handleDescriptorSync for objects that need to be re-batched
     *
     * @param change Details of the change
     * @param ogPipeline The original pipeline of the object being changed
     */
    virtual void doBatchChange(const BatchChange& change, std::uint32_t ogPipeline) override;

private:
    struct PipelineBatch {
        PipelineBatch(vk::Pipeline& pipeline)
        : pipeline(pipeline) {
            objects.reserve(Config::DefaultSceneObjectCapacity / 2);
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
            staticBatches.reserve(8);
            dynamicBatches.reserve(8);
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
        std::vector<PipelineBatch> staticBatches;
        std::vector<PipelineBatch> dynamicBatches;
        bool bindless;
    };

    struct ObjectBatch {
        ObjectBatch() { batches.reserve(8); }
        std::vector<LayoutBatch> batches;
    };

    SceneObjectStorage<SceneObject> objects;
    ObjectBatch opaqueObjects;
    ObjectBatch transparentObjects;
    std::vector<bool> staticTransCache;
    std::vector<bool> dynamicTransCache;

    void handleAddressChange(UpdateSpeed speed, SceneObject* oldBase);
};

} // namespace scene
} // namespace rc
} // namespace bl

#endif
