#include <BLIB/Render/Scenes/BasicScene.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace gfx
{
namespace scene
{
BasicScene::BasicScene(Renderer& r, std::uint32_t maxStatic, std::uint32_t maxDynamic)
: Scene(r, maxStatic, maxDynamic)
, objects(maxStatic + maxDynamic)
, transCache(maxStatic + maxDynamic, false) {}

scene::SceneObject* BasicScene::doAdd(ecs::Entity entity, com::DrawableBase& obj,
                                      std::uint32_t sceneId, UpdateSpeed updateFreq) {
    scene::SceneObject* object = &objects[sceneId];
    object->sceneId            = sceneId;
    transCache[sceneId]        = obj.containsTransparency;
    auto& batch                = obj.containsTransparency ? transparentObjects : opaqueObjects;
    vk::Pipeline& pipeline     = renderer.pipelineCache().getPipeline(obj.pipeline);

    // find or create layout batch
    LayoutBatch* layoutBatch = nullptr;
    for (LayoutBatch& lb : batch.batches) {
        if (lb.layout.rawLayout() == pipeline.pipelineLayout().rawLayout()) {
            layoutBatch = &lb;
            break;
        }
    }
    if (!layoutBatch) {
        batch.batches.emplace_back(descriptorSets, pipeline.pipelineLayout());
        layoutBatch = &batch.batches.back();
    }

    // init descriptor sets, return on fail
    for (std::uint8_t i = 0; i < layoutBatch->descriptorCount; ++i) {
        if (!layoutBatch->descriptors[i]->allocateObject(object->sceneId, entity, updateFreq)) {
            for (std::int8_t j = i - 1; j >= 0; --j) {
                layoutBatch->descriptors[j]->releaseObject(object->sceneId, entity);
            }
            return nullptr;
        }
    }

    // find or create pipeline batch
    PipelineBatch* pipelineBatch = nullptr;
    for (PipelineBatch& pb : layoutBatch->batches) {
        if (&pb.pipeline == &pipeline) {
            pipelineBatch = &pb;
            break;
        }
    }
    if (!pipelineBatch) {
        layoutBatch->batches.emplace_back(pipeline, objects.size());
        pipelineBatch = &layoutBatch->batches.back();
    }

    // add to pipeline batch
    pipelineBatch->objects.emplace_back(object);

    return object;
}

void BasicScene::doRemove(ecs::Entity, scene::SceneObject* obj, std::uint32_t pipelineId) {
    auto& batch = transCache[obj->sceneId] ? transparentObjects : opaqueObjects;

    vk::Pipeline& pipeline = renderer.pipelineCache().getPipeline(pipelineId);
    for (LayoutBatch& p : batch.batches) {
        if (p.layout.rawLayout() == pipeline.pipelineLayout().rawLayout()) {
            for (PipelineBatch& pb : p.batches) {
                if (&pb.pipeline == &pipeline) {
                    for (auto it = pb.objects.begin(); it != pb.objects.end(); ++it) {
                        if (*it == obj) {
                            pb.objects.erase(it);
                            return;
                        }
                    }
                    return;
                }
            }
            return;
        }
    }
}

void BasicScene::doBatchChange(const BatchChange& change, std::uint32_t ogPipeline) {
    // TODO - impl after refactor to batch by layout
}

void BasicScene::renderScene(scene::SceneRenderContext& ctx) {
    for (ObjectBatch* batch : std::array<ObjectBatch*, 2>{&opaqueObjects, &transparentObjects}) {
        for (LayoutBatch& lb : batch->batches) {
            const VkPipelineLayout layout = lb.layout.rawLayout();

            // bind layout descriptors
            ctx.bindDescriptors(lb.layout.rawLayout(), lb.descriptors.data(), lb.descriptorCount);

            // render each pipeline
            for (PipelineBatch& pb : lb.batches) {
                ctx.bindPipeline(pb.pipeline);
                // TODO - bind per-object here after updating Overlay
                for (SceneObject* obj : pb.objects) { ctx.renderObject(layout, *obj); }
            }
        }
    }
}

} // namespace scene
} // namespace gfx
} // namespace bl
