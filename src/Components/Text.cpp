#include <BLIB/Components/Text.hpp>

#include <BLIB/Render/Config.hpp>

namespace bl
{
namespace com
{
Text::Text()
: DrawableBase() {
    containsTransparency = true;
}

void Text::updateDrawParams() {
    const bool wasTrans = containsTransparency;

    drawParams           = vertices.getDrawParameters();
    pipeline             = rc::Config::PipelineIds::Text;
    containsTransparency = true;

    if (sceneRef.object) {
        syncDrawParamsToScene();
        if (wasTrans != containsTransparency) { rebucket(); }
    }
}

} // namespace com
} // namespace bl
