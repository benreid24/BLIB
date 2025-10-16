#ifndef BLIB_RENDER_RENDERER_BATCHEDSCENE_HPP
#define BLIB_RENDER_RENDERER_BATCHEDSCENE_HPP

#include <BLIB/Render/Descriptors/InstanceTable.hpp>
#include <BLIB/Render/Events/SceneObjectRemoved.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
#include <BLIB/Render/Scenes/SceneObjectStorage.hpp>
#include <BLIB/Signals/Emitter.hpp>

namespace bl
{
namespace rc
{
namespace scene
{
/**
 * @brief Primary scene class for the renderer. Provides batched rendering of objects by pipeline.
 *        Renders transparent objects after rendering all opaque objects
 *
 * @ingroup Renderer
 */
class BatchedScene : public Scene {
public:
    /**
     * @brief Initializes the BatchedScene
     *
     * @param engine The engine instance
     */
    BatchedScene(engine::Engine& engine);

    /**
     * @brief Unlinks allocated objects from ECS descriptor linkages
     */
    virtual ~BatchedScene();

    /**
     * @brief Renders opaque objects in the scene
     *
     * @param context Render context containing scene render data
     */
    virtual void renderOpaqueObjects(scene::SceneRenderContext& context) override;

    /**
     * @brief Renders opaque transparent in the scene
     *
     * @param context Render context containing scene render data
     */
    virtual void renderTransparentObjects(scene::SceneRenderContext& context) override;

protected:
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
     * @brief Removes the object from the scene. Also removes all children
     *
     * @param object The object to be removed
     * @param pipeline The pipeline used to render the object being removed
     */
    virtual void doObjectRemoval(scene::SceneObject* object,
                                 mat::MaterialPipeline* pipeline) override;

    /**
     * @brief Called by Scene in updateDescriptorsAndQueueTransfers for objects that need to be
     * re-batched
     *
     * @param change Details of the change
     * @param ogPipeline The original pipeline of the object being changed
     */
    virtual void doBatchChange(const BatchChange& change,
                               mat::MaterialPipeline* ogPipeline) override;

    /**
     * @brief Called when a new observer is going to render the scene
     *
     * @param target The render target that will observe the scene
     * @param observerIndex The index of the observer in the renderer
     */
    virtual void doRegisterObserver(RenderTarget* target, std::uint32_t observerIndex) override;

private:
    struct SpecializationBatch {
        const std::uint32_t specializationId;
        std::vector<SceneObject*> objectsStatic;
        std::vector<SceneObject*> objectsDynamic;

        SpecializationBatch(std::uint32_t specializationId);
        void addObject(SceneObject* sceneObject);
        bool removeObject(SceneObject* sceneObject);
    };

    struct PipelineBatch {
        PipelineBatch(const PipelineBatch& src);
        PipelineBatch(Scene* scene, mat::MaterialPipeline& pipeline);

        void initObserversMaybe(TargetTable& targets);
        void registerObserver(unsigned int index, RenderTarget& observer);
        bool addObject(ecs::Entity entity, SceneObject* sceneObject, std::uint32_t specialization);
        void addForRebatch(SceneObject* object, std::uint32_t specialization);
        void removeObject(ecs::Entity entity, SceneObject* object, std::uint32_t specialization);
        bool removeForRebatch(SceneObject* object, std::uint32_t specialization);
        void updateDescriptors(ecs::Entity entity, SceneObject* object, PipelineBatch& prevBatch);

        bool needsObserverInit;
        mat::MaterialPipeline& pipeline;
        std::array<ds::InstanceTable, cfg::Limits::MaxRenderPhases> perPhaseDescriptors;
        ctr::StaticVector<SpecializationBatch, cfg::Limits::MaxPipelineSpecializations> specBatches;
        ctr::StaticVector<ds::DescriptorSetInstance*,
                          cfg::Limits::MaxDescriptorSets * cfg::Limits::MaxRenderPhases>
            allDescriptors;
    };

    struct ObjectBatch {
        ObjectBatch() { batches.reserve(8); }
        void registerObserver(unsigned int index, RenderTarget& observer);
        PipelineBatch& getOrCreateBatch(Scene* scene, mat::MaterialPipeline& pipeline);
        PipelineBatch* getBatch(mat::MaterialPipeline* pipeline);
        PipelineBatch& getBatch(const PipelineBatch& src);
        void removeObject(ecs::Entity entity, SceneObject* object, mat::MaterialPipeline* pipeline,
                          std::uint32_t specialization);

        std::vector<PipelineBatch> batches;
    };

    struct ObjectSettingsCache {
        std::vector<bool> transparency;
        std::vector<std::uint32_t> specializations;

        ObjectSettingsCache();
        void ensureSize(std::uint32_t size);
    };

    engine::Engine& engine;
    SceneObjectStorage<SceneObject> objects;
    ObjectBatch opaqueObjects;
    ObjectBatch transparentObjects;
    ObjectSettingsCache staticCache;
    ObjectSettingsCache dynamicCache;
    sig::Emitter<event::SceneObjectRemoved> emitter;

    void handleAddressChange(UpdateSpeed speed, SceneObject* oldBase);
    void releaseObject(SceneObject* object, mat::MaterialPipeline* pipeline);
    void renderBatch(scene::SceneRenderContext& ctx, ObjectBatch& batch);
};

} // namespace scene
} // namespace rc
} // namespace bl

#endif
