#include <BLIB/Render/Renderables/Renderable.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer/Object.hpp>
#include <BLIB/Render/Renderer/PipelineInstance.hpp>
#include <BLIB/Render/Renderer/Scene.hpp>

namespace bl
{
namespace render
{
Renderable::Renderable()
: owner(nullptr)
, object(nullptr) {}

Renderable::Renderable(Renderable&& copy)
: owner(copy.owner)
, object(copy.object)
, frameData(copy.frameData) {
    copy.owner  = nullptr;
    copy.object = nullptr;
}

Renderable::~Renderable() { removeFromScene(); }

void Renderable::addToScene(Scene&, std::uint32_t, std::uint32_t) {
    // TODO - impl when scene is implemented
}

void Renderable::addToScene(PipelineInstance& pipeline) {
    removeFromScene();
    owner  = &pipeline;
    object = pipeline.createAndAddObject(this);
}

void Renderable::removeFromScene() {
    if (owner && object) { owner->removeObject(object); }
}

void Renderable::setHidden(bool h) {
    if (object) { object->hidden = h; }
}

void Renderable::markPCDirty() {
    if (object) { object->flags.markPCDirty(); }
}

void Renderable::setTextureId(std::uint32_t tid) {
    frameData.index = tid;
    markPCDirty();
}

void Renderable::setMaterialId(std::uint32_t mid) {
    frameData.index = mid;
    markPCDirty();
}

void Renderable::syncPC() {
    if (object) {
        std::unique_lock lock(mutex);
        object->frameData = frameData;
    }
}

} // namespace render
} // namespace bl
