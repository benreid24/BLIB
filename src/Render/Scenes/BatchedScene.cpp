#include <BLIB/Render/Scenes/BatchedScene.hpp>

#include <BLIB/Components/BatchSceneLink.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Render/Config/Constants.hpp>
#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace scene
{
BatchedScene::BatchedScene(engine::Engine& engine)
: Scene(engine)
, engine(engine)
, objects() {
    emitter.connect(engine.renderer().getSignalChannel());
}

BatchedScene::~BatchedScene() {
    // noop
}

scene::SceneObject* BatchedScene::doAdd(ecs::Entity entity, rcom::DrawableBase& obj,
                                        UpdateSpeed updateFreq) {
    const auto alloc            = objects.allocate(updateFreq, entity);
    const std::uint32_t sceneId = alloc.newObject->sceneKey.sceneId;
    auto& cache                 = updateFreq == UpdateSpeed::Static ? staticCache : dynamicCache;

    if (alloc.addressesChanged) {
        cache.ensureSize(alloc.newCapacity);
        handleAddressChange(updateFreq, alloc.originalBaseAddress);
    }

    cache.transparency[sceneId]    = obj.getContainsTransparency();
    cache.specializations[sceneId] = obj.getPipelineSpecialization();
    auto& batch = obj.getContainsTransparency() ? transparentObjects : opaqueObjects;
    mat::MaterialPipeline* pipeline = obj.getCurrentPipeline();

    PipelineBatch& pipelineBatch = batch.getOrCreateBatch(this, *pipeline);
    if (!pipelineBatch.addObject(entity, alloc.newObject, obj.getPipelineSpecialization())) {
        return nullptr;
    }
    pipelineBatch.initObserversMaybe(targetTable);

    // add scene link
    engine.ecs().emplaceComponent<com::BatchSceneLink>(entity, alloc.newObject->sceneKey);

    return alloc.newObject;
}

void BatchedScene::doObjectRemoval(scene::SceneObject* object, mat::MaterialPipeline* pipeline) {
    // lookup object
    ecs::Entity entity        = object->entity;
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
    emitter.emit<rc::event::SceneObjectRemoved>({this, entity});
}

void BatchedScene::releaseObject(SceneObject* object, mat::MaterialPipeline* pipeline) {
    // lookup object
    const ecs::Entity entity = object->entity;
    auto& cache = object->sceneKey.updateFreq == UpdateSpeed::Static ? staticCache : dynamicCache;
    auto& batch = cache.transparency[object->sceneKey.sceneId] ? transparentObjects : opaqueObjects;

    // remove from batch
    batch.removeObject(entity, object, pipeline, cache.specializations[object->sceneKey.sceneId]);

    // cleanup scene link
    engine.ecs().removeComponent<com::BatchSceneLink>(entity);
}

void BatchedScene::doBatchChange(const BatchChange& change, mat::MaterialPipeline* oldPipeline) {
    const ecs::Entity entity = change.changed->entity;
    const bool isStatic      = change.changed->sceneKey.updateFreq == UpdateSpeed::Static;
    auto& cache              = isStatic ? staticCache : dynamicCache;
    const bool wasTrans      = cache.transparency[change.changed->sceneKey.sceneId];
    cache.transparency[change.changed->sceneKey.sceneId] = change.newTrans;
    const std::uint32_t priorSpec = cache.specializations[change.changed->sceneKey.sceneId];
    cache.specializations[change.changed->sceneKey.sceneId] = change.newSpecialization;

    auto& oldObjectBatch = wasTrans ? transparentObjects : opaqueObjects;
    auto& newObjectBatch = change.newTrans ? transparentObjects : opaqueObjects;

    PipelineBatch* oldPipelineBatch = oldObjectBatch.getBatch(oldPipeline);
    if (!oldPipelineBatch) {
        BL_LOG_ERROR << "Failed to re-bucket entity " << entity
                     << " - Old pipeline batch not found";
        return;
    }

    // determine what changed
    const bool transChanged          = wasTrans != change.newTrans;
    const bool pipelineChanged       = oldPipeline != change.newPipeline;
    const bool specializationChanged = priorSpec != change.newSpecialization;

    if (pipelineChanged) {
        PipelineBatch& newPipelineBatch =
            newObjectBatch.getOrCreateBatch(this, *change.newPipeline);
        oldPipelineBatch->removeForRebatch(change.changed, priorSpec);
        newPipelineBatch.addForRebatch(change.changed, change.newSpecialization);
        newPipelineBatch.updateDescriptors(entity, change.changed, *oldPipelineBatch);
        newPipelineBatch.initObserversMaybe(targetTable);
    }
    else if (transChanged) {
        PipelineBatch& newPipelineBatch = newObjectBatch.getBatch(*oldPipelineBatch);
        oldPipelineBatch->removeForRebatch(change.changed, priorSpec);
        newPipelineBatch.addForRebatch(change.changed, change.newSpecialization);
    }
    else if (specializationChanged) {
        PipelineBatch& newPipelineBatch =
            newObjectBatch.getOrCreateBatch(this, *change.newPipeline);
        newPipelineBatch.removeForRebatch(change.changed, priorSpec);
        newPipelineBatch.addForRebatch(change.changed, change.newSpecialization);
        newPipelineBatch.initObserversMaybe(targetTable);
    }
}

void BatchedScene::renderBatch(scene::SceneRenderContext& ctx, ObjectBatch& batch) {
    for (auto& pipelineBatch : batch.batches) {
        for (auto& specBatch : pipelineBatch.specBatches) {
            if (!pipelineBatch.pipeline.bind(ctx.getCommandBuffer(),
                                             ctx.getRenderPhase(),
                                             ctx.currentRenderPass(),
                                             specBatch.specializationId)) {
                continue;
            }

            vk::Pipeline* pipeline = pipelineBatch.pipeline.getPipeline(ctx.getRenderPhase());
            ds::InstanceTable& descriptors =
                pipelineBatch.perPhaseDescriptors[ctx.getRenderPhase()];

            UpdateSpeed speed = UpdateSpeed::Dynamic;
            for (auto* objectBatch : {&specBatch.objectsDynamic, &specBatch.objectsStatic}) {
                ctx.bindDescriptors(pipeline->pipelineLayout().rawLayout(),
                                    speed,
                                    descriptors.get(ctx.currentObserverIndex()).data(),
                                    descriptors.getDescriptorSetCount());

                if (!descriptors.isBindless()) {
                    for (SceneObject* obj : *objectBatch) {
                        if (obj->component->isHidden()) { continue; }
                        for (std::uint8_t i = descriptors.getPerObjectStart();
                             i < descriptors.getDescriptorSetCount();
                             ++i) {
                            descriptors.get(ctx.currentObserverIndex())[i]->bindForObject(
                                ctx, pipeline->pipelineLayout().rawLayout(), i, obj->sceneKey);
                        }
                        ctx.renderObject(*obj);
                    }
                }
                else {
                    for (SceneObject* obj : *objectBatch) {
                        if (obj->component->isHidden()) { continue; }
                        ctx.renderObject(*obj);
                    }
                }

                speed = UpdateSpeed::Static;
            }
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
            for (SpecializationBatch& sb : pb.specBatches) {
                auto& objectSet =
                    speed == UpdateSpeed::Static ? sb.objectsStatic : sb.objectsDynamic;
                for (SceneObject*& so : objectSet) {
                    so = objects.rebase(speed, so, base);
                    so->updateRefToThis();
                }
            }
        }
    }
}

void BatchedScene::doRegisterObserver(RenderTarget* target, std::uint32_t observerIndex) {
    for (ObjectBatch* ob : {&opaqueObjects, &transparentObjects}) {
        for (PipelineBatch& pb : ob->batches) { pb.registerObserver(observerIndex, *target); }
    }
}

void BatchedScene::doUnregisterObserver(RenderTarget* target, std::uint32_t index) {
    objects.unlinkAll(*target->getDescriptorSetCache(this));
    for (ObjectBatch* ob : {&opaqueObjects, &transparentObjects}) {
        for (PipelineBatch& pb : ob->batches) { pb.unregisterObserver(index, *target); }
    }
}

BatchedScene::PipelineBatch::PipelineBatch(const PipelineBatch& src)
: needsObserverInit(src.needsObserverInit)
, pipeline(src.pipeline)
, perPhaseDescriptors(src.perPhaseDescriptors)
, allDescriptors(src.allDescriptors) {}

BatchedScene::PipelineBatch::PipelineBatch(Scene* scene, mat::MaterialPipeline& pipeline)
: pipeline(pipeline)
, needsObserverInit(true) {
    for (RenderPhase phase = 0; phase < cfg::Limits::MaxRenderPhaseId; ++phase) {
        vk::Pipeline* phasePipeline = pipeline.getPipeline(phase);
        if (phasePipeline) {
            auto& descriptors = perPhaseDescriptors[phase];
            descriptors.init(scene, phasePipeline->pipelineLayout());
        }
    }
}

void BatchedScene::PipelineBatch::registerObserver(unsigned int index, RenderTarget& observer) {
    const unsigned int startIndex = allDescriptors.size();
    const auto addInstanceMaybe   = [this, startIndex](ds::DescriptorSetInstance* instance) {
        for (unsigned int i = startIndex; i < allDescriptors.size(); ++i) {
            if (allDescriptors[i] == instance) { return; }
        }
        allDescriptors.emplace_back(instance);
    };

    // create descriptor sets (happens in addObserver) for all new layouts
    for (RenderPhase phase = 0; phase < cfg::Limits::MaxRenderPhaseId; ++phase) {
        vk::Pipeline* phasePipeline = pipeline.getPipeline(phase);
        if (phasePipeline) {
            perPhaseDescriptors[phase].addObserver(index, observer);
            auto& descriptors = perPhaseDescriptors[phase];
            for (unsigned int i = 0; i < descriptors.getDescriptorSetCount(); ++i) {
                addInstanceMaybe(descriptors.get(index)[i]);
            }
        }
    }

    // call allocateObject on all new sets for all existing objects
    for (SpecializationBatch& specBatch : specBatches) {
        for (unsigned int i = startIndex; i < allDescriptors.size(); ++i) {
            for (SceneObject* obj : specBatch.objectsStatic) {
                if (!allDescriptors[i]->allocateObject(obj->entity, obj->sceneKey)) {
                    BL_LOG_ERROR << "Failed to add entity to scene descriptors for new observer: "
                                 << obj->entity;
                }
            }
            for (SceneObject* obj : specBatch.objectsDynamic) {
                if (!allDescriptors[i]->allocateObject(obj->entity, obj->sceneKey)) {
                    BL_LOG_ERROR << "Failed to add entity to scene descriptors for new observer: "
                                 << obj->entity;
                }
            }
        }
    }
}

void BatchedScene::PipelineBatch::unregisterObserver(unsigned int index, RenderTarget&) {
    const auto removeFromAll = [this, index](ds::DescriptorSetInstance* instance) {
        for (unsigned int i = 0; i < allDescriptors.size(); ++i) {
            if (allDescriptors[i] == instance) {
                allDescriptors.erase(i);
                return;
            }
        }
    };

    for (RenderPhase i = 0; i < cfg::Limits::MaxRenderPhaseId; ++i) {
        vk::Pipeline* phasePipeline = pipeline.getPipeline(i);
        if (phasePipeline) {
            const auto& layoutSet = perPhaseDescriptors[i].get(index);
            for (auto* set : layoutSet) { removeFromAll(set); }
            perPhaseDescriptors[i].removeObserver(index);
        }
    }
}

void BatchedScene::PipelineBatch::initObserversMaybe(TargetTable& targets) {
    if (needsObserverInit) {
        for (unsigned int i = 0; i < targets.nextId(); ++i) {
            RenderTarget* t = targets.getTarget(i);
            if (t) { registerObserver(i, *t); }
        }
        needsObserverInit = false;
    }
}

bool BatchedScene::PipelineBatch::addObject(ecs::Entity entity, SceneObject* sceneObject,
                                            std::uint32_t specialization) {
    for (unsigned int i = 0; i < allDescriptors.size(); ++i) {
        if (!allDescriptors[i]->allocateObject(entity, sceneObject->sceneKey)) {
            BL_LOG_ERROR << "Failed to add entity " << entity
                         << " to scene due to descriptor failure";
            for (int j = static_cast<int>(i) - 1; j >= 0; --j) {
                allDescriptors[j]->releaseObject(entity, sceneObject->sceneKey);
            }
            return false;
        }
    }
    addForRebatch(sceneObject, specialization);
    return true;
}

void BatchedScene::PipelineBatch::addForRebatch(SceneObject* object, std::uint32_t specialization) {
    for (auto& b : specBatches) {
        if (b.specializationId == specialization) {
            b.addObject(object);
            return;
        }
    }
    auto& b = specBatches.emplace_back(specialization);
    b.addObject(object);
}

void BatchedScene::PipelineBatch::removeObject(ecs::Entity entity, SceneObject* object,
                                               std::uint32_t specialization) {
    if (removeForRebatch(object, specialization)) {
        for (ds::DescriptorSetInstance* set : allDescriptors) {
            set->releaseObject(entity, object->sceneKey);
        }
    }
}

bool BatchedScene::PipelineBatch::removeForRebatch(SceneObject* object,
                                                   std::uint32_t specialization) {
    for (auto& sb : specBatches) {
        if (sb.specializationId == specialization) { return sb.removeObject(object); }
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

BatchedScene::PipelineBatch& BatchedScene::ObjectBatch::getOrCreateBatch(
    Scene* scene, mat::MaterialPipeline& pipeline) {
    for (PipelineBatch& pb : batches) {
        if (&pb.pipeline == &pipeline) { return pb; }
    }
    batches.emplace_back(scene, pipeline);
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
                                             mat::MaterialPipeline* pipeline,
                                             std::uint32_t specialization) {
    for (PipelineBatch& pb : batches) {
        if (&pb.pipeline == pipeline) { pb.removeObject(entity, object, specialization); }
    }
}

BatchedScene::ObjectSettingsCache::ObjectSettingsCache() {
    transparency.resize(cfg::Constants::DefaultSceneObjectCapacity, false);
    specializations.resize(cfg::Constants::DefaultSceneObjectCapacity, 0);
}

void BatchedScene::ObjectSettingsCache::ensureSize(std::uint32_t size) {
    transparency.resize(size, false);
    specializations.resize(size, 0);
}

BatchedScene::SpecializationBatch::SpecializationBatch(std::uint32_t specializationId)
: specializationId(specializationId) {
    objectsStatic.reserve(cfg::Constants::DefaultSceneObjectCapacity / 2);
    objectsDynamic.reserve(cfg::Constants::DefaultSceneObjectCapacity / 2);
}

void BatchedScene::SpecializationBatch::addObject(SceneObject* object) {
    auto& objects =
        object->sceneKey.updateFreq == UpdateSpeed::Static ? objectsStatic : objectsDynamic;
    objects.emplace_back(object);
}

bool BatchedScene::SpecializationBatch::removeObject(SceneObject* object) {
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

} // namespace scene
} // namespace rc
} // namespace bl
