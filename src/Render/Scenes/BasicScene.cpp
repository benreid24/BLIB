#include <BLIB/Render/Scenes/BasicScene.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace gfx
{
namespace scene
{
BasicScene::BasicScene(Renderer& r)
: Scene(r)
, objects()
, staticTransCache(DefaultObjectCapacity, false)
, dynamicTransCache(DefaultObjectCapacity, false) {}

BasicScene::~BasicScene() { objects.unlinkAll(descriptorSets); }

scene::SceneObject* BasicScene::doAdd(ecs::Entity entity, com::DrawableBase& obj,
                                      UpdateSpeed updateFreq) {
    const auto alloc            = objects.allocate(updateFreq, entity);
    const std::uint32_t sceneId = alloc.newObject->sceneKey.sceneId;
    auto& transCache = updateFreq == UpdateSpeed::Static ? staticTransCache : dynamicTransCache;

    if (alloc.addressesChanged) {
        transCache.resize(alloc.newCapacity);
        handleAddressChange(updateFreq);
    }

    transCache[sceneId]    = obj.containsTransparency;
    auto& batch            = obj.containsTransparency ? transparentObjects : opaqueObjects;
    vk::Pipeline& pipeline = renderer.pipelineCache().getPipeline(obj.pipeline);

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
        if (!layoutBatch->descriptors[i]->allocateObject(entity, alloc.newObject->sceneKey)) {
            for (std::int8_t j = i - 1; j >= 0; --j) {
                layoutBatch->descriptors[j]->releaseObject(entity, alloc.newObject->sceneKey);
            }
            return nullptr;
        }
    }

    // find or create pipeline batch
    auto& pipelineBatches        = updateFreq == UpdateSpeed::Static ? layoutBatch->staticBatches :
                                                                       layoutBatch->dynamicBatches;
    PipelineBatch* pipelineBatch = nullptr;
    for (PipelineBatch& pb : pipelineBatches) {
        if (&pb.pipeline == &pipeline) {
            pipelineBatch = &pb;
            break;
        }
    }
    if (!pipelineBatch) {
        pipelineBatches.emplace_back(pipeline);
        pipelineBatch = &pipelineBatches.back();
    }

    // add to pipeline batch
    pipelineBatch->objects.emplace_back(alloc.newObject);

    return alloc.newObject;
}

void BasicScene::doRemove(scene::SceneObject* object, std::uint32_t pipelineId) {
    const ecs::Entity entity = objects.getObjectEntity(object->sceneKey);
    auto& transCache =
        object->sceneKey.updateFreq == UpdateSpeed::Static ? staticTransCache : dynamicTransCache;
    auto& batch = transCache[object->sceneKey.sceneId] ? transparentObjects : opaqueObjects;

    vk::Pipeline& pipeline = renderer.pipelineCache().getPipeline(pipelineId);
    for (LayoutBatch& p : batch.batches) {
        if (p.layout.rawLayout() == pipeline.pipelineLayout().rawLayout()) {
            auto& pipelineBatches = object->sceneKey.updateFreq == UpdateSpeed::Static ?
                                        p.staticBatches :
                                        p.dynamicBatches;
            for (PipelineBatch& pb : pipelineBatches) {
                if (&pb.pipeline == &pipeline) {
                    for (auto it = pb.objects.begin(); it != pb.objects.end(); ++it) {
                        if (*it == object) {
                            pb.objects.erase(it);
                            goto removeDescriptors;
                        }
                    }
                    break;
                }
            }
        removeDescriptors:
            for (std::uint8_t i = 0; i < p.descriptorCount; ++i) {
                p.descriptors[i]->releaseObject(entity, object->sceneKey);
            }
            return;
        }
    }
}

void BasicScene::doBatchChange(const BatchChange& change, std::uint32_t ogPipeline) {
    const bool isStatic = change.changed->sceneKey.updateFreq == UpdateSpeed::Static;
    auto& transCache    = isStatic ? staticTransCache : dynamicTransCache;
    const bool wasTrans = transCache[change.changed->sceneKey.sceneId];

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
                oldLayoutBatch        = &lb;
                auto& pipelineBatches = isStatic ? lb.staticBatches : lb.dynamicBatches;
                for (PipelineBatch& pb : pipelineBatches) {
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
            layoutBatch = &batch.batches.emplace_back(descriptorSets, newPipeline.pipelineLayout());
        }

        // find or create pipeline batch
        auto& pipelineBatches = isStatic ? layoutBatch->staticBatches : layoutBatch->dynamicBatches;
        PipelineBatch* pipelineBatch = nullptr;
        for (PipelineBatch& pb : pipelineBatches) {
            if (&pb.pipeline == &newPipeline) {
                pipelineBatch = &pb;
                break;
            }
        }
        if (!pipelineBatch) { pipelineBatch = &pipelineBatches.emplace_back(newPipeline); }

        // add to pipeline batch
        pipelineBatch->objects.emplace_back(change.changed);

        // update descriptors if layout changed
        if (layoutChanged) {
            SceneObject& object      = *change.changed;
            const ecs::Entity entity = objects.getObjectEntity(object.sceneKey);
            newPipeline.pipelineLayout().updateDescriptorSets(descriptorSets,
                                                              oldLayoutBatch->descriptors.data(),
                                                              oldLayoutBatch->descriptorCount,
                                                              entity,
                                                              object.sceneKey.sceneId,
                                                              object.sceneKey.updateFreq);
        }
    }
}

void BasicScene::renderScene(scene::SceneRenderContext& ctx) {
    for (ObjectBatch* batch : std::array<ObjectBatch*, 2>{&opaqueObjects, &transparentObjects}) {
        for (LayoutBatch& lb : batch->batches) {
            const VkPipelineLayout layout = lb.layout.rawLayout();
            UpdateSpeed speed             = UpdateSpeed::Dynamic;

            for (auto* pipelineBatches : std::array<std::vector<PipelineBatch>*, 2>{
                     &lb.dynamicBatches, &lb.staticBatches}) {
                // bind layout descriptors
                ctx.bindDescriptors(layout, speed, lb.descriptors.data(), lb.descriptorCount);

                // render each pipeline
                if (!lb.bindless) {
                    for (PipelineBatch& pb : *pipelineBatches) {
                        ctx.bindPipeline(pb.pipeline);
                        for (SceneObject* obj : pb.objects) {
                            for (std::uint8_t i = lb.perObjStart; i < lb.descriptorCount; ++i) {
                                lb.descriptors[i]->bindForObject(ctx, layout, i, obj->sceneKey);
                            }
                            ctx.renderObject(*obj);
                        }
                    }
                }
                else {
                    for (PipelineBatch& pb : *pipelineBatches) {
                        ctx.bindPipeline(pb.pipeline);
                        for (SceneObject* obj : pb.objects) { ctx.renderObject(*obj); }
                    }
                }

                speed = UpdateSpeed::Static;
            }
        }
    }
}

} // namespace scene
} // namespace gfx
} // namespace bl
