#include <BLIB/Render/Scenes/BatchedScene.hpp>

#include <BLIB/Components/BatchSceneLink.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Render/Events/SceneObjectRemoved.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace scene
{
BatchedScene::BatchedScene(engine::Engine& engine)
: Scene(engine, objects.makeEntityCallback())
, engine(engine)
, objects()
, staticTransCache(Config::DefaultSceneObjectCapacity, false)
, dynamicTransCache(Config::DefaultSceneObjectCapacity, false) {}

BatchedScene::~BatchedScene() { objects.unlinkAll(descriptorSets); }

scene::SceneObject* BatchedScene::doAdd(ecs::Entity entity, rcom::DrawableBase& obj,
                                        UpdateSpeed updateFreq) {
    const auto alloc            = objects.allocate(updateFreq, entity);
    const std::uint32_t sceneId = alloc.newObject->sceneKey.sceneId;
    auto& transCache = updateFreq == UpdateSpeed::Static ? staticTransCache : dynamicTransCache;

    if (alloc.addressesChanged) {
        transCache.resize(alloc.newCapacity);
        handleAddressChange(updateFreq, alloc.originalBaseAddress);
    }

    transCache[sceneId] = obj.getContainsTransparency();
    auto& batch         = obj.getContainsTransparency() ? transparentObjects : opaqueObjects;
    mat::MaterialPipeline* pipeline = obj.getCurrentPipeline();

    PipelineBatch& pipelineBatch = batch.getBatch(descriptorSets, *pipeline);
    if (!pipelineBatch.addObject(entity, alloc.newObject)) { return nullptr; }

    // add scene link
    engine.ecs().emplaceComponent<com::BatchSceneLink>(entity, alloc.newObject->sceneKey);

    return alloc.newObject;
}

void BatchedScene::doObjectRemoval(scene::SceneObject* object, mat::MaterialPipeline* pipeline) {
    // lookup object
    const ecs::Entity entity  = objects.getObjectEntity(object->sceneKey);
    com::BatchSceneLink* link = engine.ecs().getComponent<com::BatchSceneLink>(entity);

    // remove children
    if (link) {
        for (com::BatchSceneLink* child : link->getChildren()) {
            removeObject(&objects.getObject(child->key));
        }
    }
    else { BL_LOG_WARN << "Failed to find BatchSceneLink for entity: " << entity; }

    // release object
    releaseObject(object, pipeline);

    // fire event
    bl::event::Dispatcher::dispatch<rc::event::SceneObjectRemoved>({this, entity});
}

void BatchedScene::releaseObject(SceneObject* object, mat::MaterialPipeline* pipeline) {
    // lookup object
    const ecs::Entity entity = objects.getObjectEntity(object->sceneKey);
    auto& transCache =
        object->sceneKey.updateFreq == UpdateSpeed::Static ? staticTransCache : dynamicTransCache;
    auto& batch = transCache[object->sceneKey.sceneId] ? transparentObjects : opaqueObjects;

    // remove from batch
    batch.removeObject(entity, object, pipeline);

    // cleanup scene link
    engine.ecs().removeComponent<com::BatchSceneLink>(entity);
}

void BatchedScene::doBatchChange(const BatchChange& change, mat::MaterialPipeline* oldPipeline) {
    const ecs::Entity entity = objects.getObjectEntity(change.changed->sceneKey);
    const bool isStatic      = change.changed->sceneKey.updateFreq == UpdateSpeed::Static;
    auto& transCache         = isStatic ? staticTransCache : dynamicTransCache;
    const bool wasTrans      = transCache[change.changed->sceneKey.sceneId];
    transCache[change.changed->sceneKey.sceneId] = change.newTrans;

    auto& oldObjectBatch = wasTrans ? transparentObjects : opaqueObjects;
    auto& newObjectBatch = change.newTrans ? transparentObjects : opaqueObjects;

    PipelineBatch* oldPipelineBatch = oldObjectBatch.getBatch(oldPipeline);
    if (!oldPipelineBatch) {
        BL_LOG_ERROR << "Failed to re-bucket entity " << entity
                     << " - Old pipeline batch not found";
        return;
    }

    // determine what changed
    const bool transChanged    = wasTrans != change.newTrans;
    const bool pipelineChanged = oldPipeline != change.newPipeline;

    if (pipelineChanged) {
        PipelineBatch& newPipelineBatch =
            newObjectBatch.getBatch(descriptorSets, *change.newPipeline);
        oldPipelineBatch->removeForRebatch(change.changed);
        newPipelineBatch.addForRebatch(change.changed);
        newPipelineBatch.updateDescriptors(entity, change.changed, *oldPipelineBatch);
    }
    else if (transChanged) {
        PipelineBatch& newPipelineBatch = newObjectBatch.getBatch(*oldPipelineBatch);
        oldPipelineBatch->removeForRebatch(change.changed);
        newPipelineBatch.addForRebatch(change.changed);
    }
}

void BatchedScene::renderBatch(scene::SceneRenderContext& ctx, ObjectBatch& batch) {
    for (auto& pipelineBatch : batch.batches) {
        vk::Pipeline* pipeline = pipelineBatch.pipeline.getPipeline(ctx.getRenderPhase());
        RenderPhaseDescriptors& descriptors =
            pipelineBatch.perPhaseDescriptors[renderPhaseIndex(ctx.getRenderPhase())];
        if (!pipeline) { continue; }

        pipeline->bind(ctx.getCommandBuffer(), ctx.currentRenderPass());

        UpdateSpeed speed = UpdateSpeed::Dynamic;
        for (auto* objectBatch : {&pipelineBatch.objectsDynamic, &pipelineBatch.objectsStatic}) {
            ctx.bindDescriptors(pipeline->pipelineLayout().rawLayout(),
                                speed,
                                descriptors.descriptors.data(),
                                descriptors.descriptorCount);

            if (!descriptors.bindless) {
                for (SceneObject* obj : *objectBatch) {
                    if (obj->hidden) { continue; }
                    for (std::uint8_t i = descriptors.perObjStart; i < descriptors.descriptorCount;
                         ++i) {
                        descriptors.descriptors[i]->bindForObject(
                            ctx, pipeline->pipelineLayout().rawLayout(), i, obj->sceneKey);
                    }
                    ctx.renderObject(*obj);
                }
            }
            else {
                for (SceneObject* obj : *objectBatch) {
                    if (obj->hidden) { continue; }
                    ctx.renderObject(*obj);
                }
            }

            speed = UpdateSpeed::Static;
        }
    }
}

void BatchedScene::renderTransparentObjects(scene::SceneRenderContext& ctx) {
    renderBatch(ctx, transparentObjects);
}

void BatchedScene::renderOpaqueObjects(scene::SceneRenderContext& ctx) {
    renderBatch(ctx, opaqueObjects);
}

void BatchedScene::handleAddressChange(UpdateSpeed speed, SceneObject* base) {
    for (ObjectBatch* ob : {&opaqueObjects, &transparentObjects}) {
        for (PipelineBatch& pb : ob->batches) {
            auto& objectSet = speed == UpdateSpeed::Static ? pb.objectsStatic : pb.objectsDynamic;
            for (SceneObject*& so : objectSet) {
                so = objects.rebase(speed, so, base);
                so->updateRefToThis();
            }
        }
    }
}

void BatchedScene::RenderPhaseDescriptors::init(ds::DescriptorSetInstanceCache& descriptorCache,
                                                const vk::Pipeline* pipeline) {
    bindless        = true;
    descriptorCount = 0;
    perObjStart     = 4;
    if (pipeline) {
        descriptorCount =
            pipeline->pipelineLayout().initDescriptorSets(descriptorCache, descriptors.data());
        for (std::uint8_t i = 0; i < descriptorCount; ++i) {
            if (!descriptors[i]->isBindless()) {
                bindless    = false;
                perObjStart = i;
                break;
            }
        }
    }
}

BatchedScene::PipelineBatch::PipelineBatch(const PipelineBatch& src)
: pipeline(src.pipeline)
, perPhaseDescriptors(src.perPhaseDescriptors)
, allDescriptors(src.allDescriptors) {
    objectsStatic.reserve(Config::DefaultSceneObjectCapacity / 2);
    objectsDynamic.reserve(Config::DefaultSceneObjectCapacity / 2);
}

BatchedScene::PipelineBatch::PipelineBatch(ds::DescriptorSetInstanceCache& descriptorCache,
                                           mat::MaterialPipeline& pipeline)
: pipeline(pipeline) {
    objectsStatic.reserve(Config::DefaultSceneObjectCapacity / 2);
    objectsDynamic.reserve(Config::DefaultSceneObjectCapacity / 2);
    for (const RenderPhase phase : AllRenderPhases) {
        auto& descriptors = perPhaseDescriptors[renderPhaseIndex(phase)];
        descriptors.init(descriptorCache, pipeline.getPipeline(phase));
        for (std::uint32_t i = 0; i < descriptors.descriptorCount; ++i) {
            bool found = false;
            for (ds::DescriptorSetInstance* set : allDescriptors) {
                if (descriptors.descriptors[i] == set) {
                    found = true;
                    break;
                }
            }
            if (!found) { allDescriptors.emplace_back(descriptors.descriptors[i]); }
        }
    }
}

bool BatchedScene::PipelineBatch::addObject(ecs::Entity entity, SceneObject* sceneObject) {
    for (unsigned int i = 0; i < allDescriptors.size(); ++i) {
        if (!allDescriptors[i]->allocateObject(entity, sceneObject->sceneKey)) {
            BL_LOG_ERROR << "Failed to add entity " << entity
                         << " to scene due to descriptor failure";
            for (int j = i - 1; i >= 0; --j) {
                allDescriptors[j]->releaseObject(entity, sceneObject->sceneKey);
            }
            return false;
        }
    }
    addForRebatch(sceneObject);
    return true;
}

void BatchedScene::PipelineBatch::addForRebatch(SceneObject* object) {
    auto& objects =
        object->sceneKey.updateFreq == UpdateSpeed::Static ? objectsStatic : objectsDynamic;
    objects.emplace_back(object);
}

void BatchedScene::PipelineBatch::removeObject(ecs::Entity entity, SceneObject* object) {
    if (removeForRebatch(object)) {
        for (ds::DescriptorSetInstance* set : allDescriptors) {
            set->releaseObject(entity, object->sceneKey);
        }
    }
}

bool BatchedScene::PipelineBatch::removeForRebatch(SceneObject* object) {
    auto& batch =
        object->sceneKey.updateFreq == UpdateSpeed::Static ? objectsStatic : objectsDynamic;
    for (auto it = batch.begin(); it != batch.end(); ++it) {
        if (*it == object) {
            batch.erase(it);
            return true;
        }
    }
    return false;
}

void BatchedScene::PipelineBatch::updateDescriptors(ecs::Entity entity, SceneObject* object,
                                                    PipelineBatch& prevBatch) {
    using AllCtr    = decltype(allDescriptors);
    const auto find = [](const AllCtr& all, ds::DescriptorSetInstance* set) -> bool {
        for (ds::DescriptorSetInstance* s : all) {
            if (s == set) { return true; }
        }
        return false;
    };

    // remove from old descriptors not in new
    for (ds::DescriptorSetInstance* set : prevBatch.allDescriptors) {
        if (!find(allDescriptors, set)) { set->releaseObject(entity, object->sceneKey); }
    }

    // add to new descriptors not in old
    for (ds::DescriptorSetInstance* set : allDescriptors) {
        if (!find(prevBatch.allDescriptors, set)) {
            if (!set->allocateObject(entity, object->sceneKey)) {
                BL_LOG_ERROR << "Failed to update descriptors for entity " << entity
                             << " in scene due to descriptor failure";
            }
        }
    }
}

BatchedScene::PipelineBatch& BatchedScene::ObjectBatch::getBatch(
    ds::DescriptorSetInstanceCache& descriptorCache, mat::MaterialPipeline& pipeline) {
    for (PipelineBatch& pb : batches) {
        if (&pb.pipeline == &pipeline) { return pb; }
    }
    batches.emplace_back(descriptorCache, pipeline);
    return batches.back();
}

BatchedScene::PipelineBatch* BatchedScene::ObjectBatch::getBatch(mat::MaterialPipeline* pipeline) {
    for (PipelineBatch& pb : batches) {
        if (&pb.pipeline == pipeline) { return &pb; }
    }
    return nullptr;
}

BatchedScene::PipelineBatch& BatchedScene::ObjectBatch::getBatch(const PipelineBatch& src) {
    for (PipelineBatch& pb : batches) {
        if (&pb.pipeline == &src.pipeline) { return pb; }
    }
    batches.emplace_back(src);
    return batches.back();
}

void BatchedScene::ObjectBatch::removeObject(ecs::Entity entity, SceneObject* object,
                                             mat::MaterialPipeline* pipeline) {
    for (PipelineBatch& pb : batches) {
        if (&pb.pipeline == pipeline) { pb.removeObject(entity, object); }
    }
}

} // namespace scene
} // namespace rc
} // namespace bl
