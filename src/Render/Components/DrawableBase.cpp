#include <BLIB/Render/Components/DrawableBase.hpp>

#include <BLIB/Render/Scenes/Scene.hpp>

namespace bl
{
namespace gfx
{
namespace com
{
DrawableBase::DrawableBase()
: pipeline(PipelineNotSet)
, containsTransparency(false) {}

void DrawableBase::syncDrawParamsToScene() {
#ifdef BLIB_DEBUG
    if (!sceneRef.object) {
        BL_LOG_ERROR << "Called syncDrawParamsToScene on object not in scene";
        return;
    }
#endif

    sceneRef.object->drawParams = drawParams;
}

void DrawableBase::setHidden(bool hide) {
#ifdef BLIB_DEBUG
    if (!sceneRef.object) {
        BL_LOG_ERROR << "Called setHidden on object not in scene";
        return;
    }
#endif

    sceneRef.object->hidden = hide;
}

void DrawableBase::setPipeline(std::uint32_t pid) {
    pipeline = pid;
    rebucket();
}

void DrawableBase::rebucket() {
    if (sceneRef.scene) { sceneRef.scene->rebucketObject(*this); }
}

} // namespace com
} // namespace gfx
} // namespace bl
