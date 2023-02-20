#include <BLIB/Render/Renderables/Renderable.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer/Scene.hpp>
#include <BLIB/Render/Renderer/SceneObject.hpp>

namespace bl
{
namespace render
{
Renderable::Renderable()
: owner(nullptr)
, hidden(true) {}

Renderable::Renderable(Renderable&& copy)
: owner(copy.owner)
, object(copy.object)
, frameData(copy.frameData)
, hidden(copy.hidden)
, stageMembership(copy.stageMembership) {
    copy.owner = nullptr;
    copy.object.release();
}

Renderable::~Renderable() {
    if (owner) { removeFromScene(); }
}

void Renderable::addToScene(Scene& scene) {
    if (owner) { removeFromScene(); }

    std::unique_lock lock(mutex);
    object = scene.createAndAddObject(this);
    onSceneAdd();
    stageMembership.prepareForNewScene();
    object->flags.markAllDirty();
    object->hidden = hidden;
}

void Renderable::onSceneAdd() {
    // do nothing
}

void Renderable::removeFromScene() {
    std::unique_lock lock(mutex);
    owner->removeObject(object);
    owner = nullptr;
    object.release();
}

void Renderable::setHidden(bool h) {
    hidden = h;
    if (object.valid()) { object->hidden = h; }
}

bool Renderable::isHidden() const { return hidden; }

const glm::mat4& Renderable::getTransform() const { return frameData.transform; }

void Renderable::markPCDirty() {
    if (object.valid()) { object->flags.markPCDirty(); }
}

void Renderable::addOrSetStagePipeline(std::uint32_t renderStageId, std::uint32_t pipelineId) {
    std::unique_lock lock(mutex);
    stageMembership.addOrSetStagePipeline(renderStageId, pipelineId);
    if (object.valid() && stageMembership.hasDiff()) { object->flags.markRenderStagesDirty(); }
}

void Renderable::removeFromStage(std::uint32_t stage) {
    std::unique_lock lock(mutex);
    stageMembership.removeFromStage(stage);
    if (object.valid() && stageMembership.hasDiff()) { object->flags.markRenderStagesDirty(); }
}

void Renderable::setDrawParameters(const DrawParameters& dp) {
    std::unique_lock lock(mutex);
    drawParams = dp;
    if (object.valid()) { object->flags.markDrawParamsDirty(); }
}

void Renderable::setTextureId(std::uint32_t tid) {
    frameData.index = tid;
    markPCDirty();
}

void Renderable::setMaterialId(std::uint32_t mid) {
    frameData.index = mid;
    markPCDirty();
}

void Renderable::setTransform(const glm::mat4& t) {
    std::unique_lock lock(mutex);
    frameData.transform = t;
    markPCDirty();
}

void Renderable::syncPC() {
    std::unique_lock lock(mutex);
    object->frameData = frameData;
}

void Renderable::syncDrawParams() {
    std::unique_lock lock(mutex);
    object->drawParams = drawParams;
}

} // namespace render
} // namespace bl
