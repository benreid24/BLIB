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
    const bool wasTrans = transCache[change.changed->sceneId];

    vk::Pipeline& newPipeline = renderer.pipelineCache().getPipeline(change.newPipeline);
    vk::Pipeline& oldPipeline = renderer.pipelineCache().getPipeline(ogPipeline);

    // determine what changed
    const bool transChanged    = wasTrans != change.newTrans;
    const bool pipelineChanged = ogPipeline != change.newPipeline;
    bool layoutChanged         = false;

    // determine if layout changed
    if (pipelineChanged) {
        layoutChanged =
            newPipeline.pipelineLayout().rawLayout() != oldPipeline.pipelineLayout().rawLayout();
    }

    // re-bucket if trans, layout, or pipeline changed
    if (transChanged || layoutChanged || pipelineChanged) {
        // remove from old bucket
        LayoutBatch* oldLayoutBatch = nullptr;
        auto& oldObjectBatch        = wasTrans ? transparentObjects : opaqueObjects;
        for (LayoutBatch& lb : oldObjectBatch.batches) {
            if (lb.layout.rawLayout() == oldPipeline.pipelineLayout().rawLayout()) {
                oldLayoutBatch = &lb;
                for (PipelineBatch& pb : lb.batches) {
                    if (&pb.pipeline == &oldPipeline) {
                        for (auto it = pb.objects.begin(); it != pb.objects.end(); ++it) {
                            if (*it == change.changed) {
                                pb.objects.erase(it);
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }

        auto& batch = change.newTrans ? transparentObjects : opaqueObjects;

        // find or create new layout batch
        LayoutBatch* layoutBatch = nullptr;
        for (LayoutBatch& lb : batch.batches) {
            if (lb.layout.rawLayout() == newPipeline.pipelineLayout().rawLayout()) {
                layoutBatch = &lb;
                break;
            }
        }
        if (!layoutBatch) {
            batch.batches.emplace_back(descriptorSets, newPipeline.pipelineLayout());
            layoutBatch = &batch.batches.back();
        }

        // find or create pipeline batch
        PipelineBatch* pipelineBatch = nullptr;
        for (PipelineBatch& pb : layoutBatch->batches) {
            if (&pb.pipeline == &newPipeline) {
                pipelineBatch = &pb;
                break;
            }
        }
        if (!pipelineBatch) {
            layoutBatch->batches.emplace_back(newPipeline, objects.size());
            pipelineBatch = &layoutBatch->batches.back();
        }

        // add to pipeline batch
        pipelineBatch->objects.emplace_back(change.changed);

        // update descriptors if layout changed
        if (layoutChanged) {
            SceneObject& object      = *change.changed;
            const ecs::Entity entity = getEntityFromId(object.sceneId);
            const UpdateSpeed speed  = getObjectSpeed(object.sceneId);
            newPipeline.pipelineLayout().updateDescriptorSets(descriptorSets,
                                                              oldLayoutBatch->descriptors.data(),
                                                              oldLayoutBatch->descriptorCount,
                                                              entity,
                                                              object.sceneId,
                                                              speed);
        }
    }
}

void BasicScene::renderScene(scene::SceneRenderContext& ctx) {
    for (ObjectBatch* batch : std::array<ObjectBatch*, 2>{&opaqueObjects, &transparentObjects}) {
        for (LayoutBatch& lb : batch->batches) {
            const VkPipelineLayout layout = lb.layout.rawLayout();

            // bind layout descriptors
            ctx.bindDescriptors(layout, lb.descriptors.data(), lb.descriptorCount);

            // render each pipeline
            if (!lb.bindless) {
                for (PipelineBatch& pb : lb.batches) {
                    ctx.bindPipeline(pb.pipeline);
                    for (SceneObject* obj : pb.objects) {
                        for (std::uint8_t i = lb.perObjStart; i < lb.descriptorCount; ++i) {
                            lb.descriptors[i]->bindForObject(ctx, layout, i, obj->sceneId);
                        }
                        ctx.renderObject(*obj);
                    }
                }
            }
            else {
                for (PipelineBatch& pb : lb.batches) {
                    ctx.bindPipeline(pb.pipeline);
                    for (SceneObject* obj : pb.objects) { ctx.renderObject(*obj); }
                }
            }
        }
    }
}

} // namespace scene
} // namespace gfx
} // namespace bl
