#include <BLIB/Render/Components/DrawableBase.hpp>

#include <BLIB/Render/Scenes/Scene.hpp>

namespace bl
{
namespace rc
{
namespace rcom
{
DrawableBase::DrawableBase()
: pipeline(PipelineNotSet)
, containsTransparency(false)
, hidden(false) {}

void DrawableBase::syncDrawParamsToScene() {
#ifdef BLIB_DEBUG
    if (!sceneRef.object) {
        BL_LOG_ERROR << "Called syncDrawParamsToScene on object not in scene";
        return;
    }
#endif

    sceneRef.object->drawParams = drawParams;
    sceneRef.object->hidden     = hidden;
}

void DrawableBase::setHidden(bool hide) {
    if (sceneRef.object) { sceneRef.object->hidden = hide; }
    hidden = hide;
}

void DrawableBase::setPipeline(std::uint32_t pid) {
    pipeline = pid;
    rebucket();
}

void DrawableBase::rebucket() {
    if (sceneRef.scene) { sceneRef.scene->rebucketObject(*this); }
}

} // namespace rcom
} // namespace rc
} // namespace bl
