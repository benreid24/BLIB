#include <BLIB/Components/Text.hpp>

#include <BLIB/Render/Config.hpp>

namespace bl
{
namespace com
{
void Text::updateDrawParams() {
    const bool wasTrans = containsTransparency;

    drawParams           = vertices.getDrawParameters();
    pipeline             = rc::Config::PipelineIds::Text;
    containsTransparency = false;
    for (unsigned int i = 0; i < vertices.vertexCount(); ++i) {
        if (vertices.vertices()[i].color.w > 0.f && vertices.vertices()[i].color.w < 1.f) {
            containsTransparency = true;
            break;
        }
    }

    if (sceneRef.object) {
        syncDrawParamsToScene();
        if (wasTrans != containsTransparency) { rebucket(); }
    }
}

} // namespace com
} // namespace bl
