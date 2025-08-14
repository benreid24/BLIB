#include <BLIB/Render/Graph/Tasks/Outline3DTask.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Config/PipelineIds.hpp>
#include <BLIB/Render/Config/RenderPhases.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/TaskIds.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
Outline3DTask::Outline3DTask()
: Task(rg::TaskIds::Outline3DTask)
, registry(nullptr)
, scene(nullptr) {
    assetTags.outputs.emplace_back(rg::TaskOutput({rg::AssetTags::RenderedSceneOutputHDR,
                                                   rg::AssetTags::RenderedSceneOutput,
                                                   rg::AssetTags::FinalFrameOutput},
                                                  {rg::TaskOutput::CreatedByOtherTask,
                                                   rg::TaskOutput::CreatedByOtherTask,
                                                   rg::TaskOutput::CreatedExternally},
                                                  {rg::TaskOutput::Shared},
                                                  rg::TaskOutput::Last,
                                                  {rg::TaskIds::ForwardRenderTransparentTask}));
}

void Outline3DTask::create(engine::Engine& engine, Renderer& renderer, Scene* s) {
    registry = &engine.ecs();
    scene    = s;

    vk::Pipeline* standard = &renderer.pipelineCache().getPipeline(cfg::PipelineIds::Outline3D);
    vk::Pipeline* skinned =
        &renderer.pipelineCache().getPipeline(cfg::PipelineIds::Outline3DSkinned);
    standardPipeline.init(standard, scene->getDescriptorSets());
    skinnedPipeline.init(skinned, scene->getDescriptorSets());

    // load outlines that were added before this call
    ecs::Transaction<ecs::tx::EntityUnlocked, ecs::tx::ComponentRead<com::Rendered, com::Outline>>
        tx(engine.ecs());
    engine.ecs().getAllComponents<com::Outline>().forEach(
        [this, &tx](ecs::Entity entity, com::Outline& outline) {
            com::Rendered* rendered = registry->getComponent<com::Rendered>(entity, tx);
            if (rendered && rendered->getScene() == scene) {
                addObject(entity, rendered, &outline);
            }
        },
        tx);

    subscribe(engine.ecs().getSignalChannel());
}

void Outline3DTask::onGraphInit() {
    // noop
}

void Outline3DTask::execute(const rg::ExecutionContext& ctx, rg::Asset* output) {
    FramebufferAsset* fb = dynamic_cast<FramebufferAsset*>(output);
    if (!fb) { throw std::runtime_error("Got invalid output for outline task"); }

    scene::SceneRenderContext sceneCtx(ctx.commandBuffer,
                                       ctx.observerIndex,
                                       fb->getViewport(),
                                       cfg::RenderPhases::Outline,
                                       fb->getRenderPassId(),
                                       ctx.renderingToRenderTexture);

    const auto pushConstants = [&sceneCtx](const Outlined& obj, VkPipelineLayout layout) {
        float localScale = obj.outline->getThickness();
        vkCmdPushConstants(sceneCtx.getCommandBuffer(),
                           layout,
                           VK_SHADER_STAGE_VERTEX_BIT,
                           0,
                           sizeof(float),
                           &localScale);
        vkCmdPushConstants(sceneCtx.getCommandBuffer(),
                           layout,
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                           16,
                           sizeof(glm::vec4),
                           &obj.outline->getColor());
    };

    VkPipelineLayout standardLayout = standardPipeline.getPipeline().pipelineLayout().rawLayout();
    VkPipelineLayout skinnedLayout  = skinnedPipeline.getPipeline().pipelineLayout().rawLayout();

    if (!outlinedObjectsDynamic.empty()) {
        standardPipeline.bind(sceneCtx, 0, UpdateSpeed::Dynamic);
        for (const Outlined& obj : outlinedObjectsDynamic) {
            pushConstants(obj, standardLayout);
            sceneCtx.renderObject(*obj.drawable->getComponent());
        }
    }
    if (!outlinedObjectsStatic.empty()) {
        standardPipeline.bind(sceneCtx, 0, UpdateSpeed::Static);
        for (const Outlined& obj : outlinedObjectsStatic) {
            pushConstants(obj, standardLayout);
            sceneCtx.renderObject(*obj.drawable->getComponent());
        }
    }
    if (!outlinedSkinnedObjectsDynamic.empty()) {
        skinnedPipeline.bind(sceneCtx, 0, UpdateSpeed::Dynamic);
        for (const Outlined& obj : outlinedSkinnedObjectsDynamic) {
            pushConstants(obj, skinnedLayout);
            sceneCtx.renderObject(*obj.drawable->getComponent());
        }
    }
    if (!outlinedSkinnedObjectsStatic.empty()) {
        skinnedPipeline.bind(sceneCtx, 0, UpdateSpeed::Static);
        for (const Outlined& obj : outlinedSkinnedObjectsStatic) {
            pushConstants(obj, skinnedLayout);
            sceneCtx.renderObject(*obj.drawable->getComponent());
        }
    }
}

void Outline3DTask::update(float) {
    // noop
}

void Outline3DTask::process(const ecs::event::ComponentAdded<com::Outline>& event) {
    com::Rendered* rendered = registry->getComponent<com::Rendered>(event.entity);
    if (rendered && rendered->getScene() == scene) {
        addObject(event.entity, rendered, &event.component);
    }
}

void Outline3DTask::process(const ecs::event::ComponentAdded<com::Rendered>& event) {
    if (event.component.getScene() == scene) {
        com::Outline* outline = registry->getComponent<com::Outline>(event.entity);
        if (outline) { addObject(event.entity, &event.component, outline); }
    }
}

void Outline3DTask::process(const ecs::event::ComponentRemoved<com::Outline>& event) {
    removeObject(event.entity);
}

void Outline3DTask::process(const ecs::event::ComponentRemoved<com::Rendered>& event) {
    removeObject(event.entity);
}

void Outline3DTask::addObject(ecs::Entity entity, com::Rendered* drawable, com::Outline* outline) {
    vk::Pipeline* pipeline =
        drawable->getComponent()->getCurrentPipeline()->getPipeline(cfg::RenderPhases::Outline);
    if (pipeline == &standardPipeline.getPipeline()) {
        auto& list = drawable->getUpdateSpeed() == UpdateSpeed::Dynamic ? outlinedObjectsDynamic :
                                                                          outlinedObjectsStatic;
        list.emplace_back(Outlined{.drawable = drawable, .outline = outline});
    }
    else if (pipeline == &skinnedPipeline.getPipeline()) {
        auto& list = drawable->getUpdateSpeed() == UpdateSpeed::Dynamic ?
                         outlinedSkinnedObjectsDynamic :
                         outlinedSkinnedObjectsStatic;
        list.emplace_back(Outlined{.drawable = drawable, .outline = outline});
    }
    else {
        BL_LOG_WARN << "Entity has outline but is using pipeline that does not support it: "
                    << entity;
    }
}

void Outline3DTask::removeObject(ecs::Entity entity) {
    const auto remove = [&entity](std::vector<Outlined>& list) -> bool {
        for (auto it = list.begin(); it != list.end(); ++it) {
            if (it->drawable->getEntity() == entity) {
                list.erase(it);
                return true;
            }
        }
        return false;
    };

    // fetching & checking likely does not save us any time in 90% of cases so just search
    if (remove(outlinedObjectsDynamic)) { return; }
    if (remove(outlinedSkinnedObjectsDynamic)) { return; }
    if (remove(outlinedObjectsStatic)) { return; }
    remove(outlinedSkinnedObjectsStatic);
}

} // namespace rgi
} // namespace rc
} // namespace bl
