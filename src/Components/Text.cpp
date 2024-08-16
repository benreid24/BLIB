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

void Text::updateDrawParams(std::uint32_t vc) {
    const bool wasTrans = containsTransparency;

    drawParams             = vertices.getDrawParameters();
    drawParams.vertexCount = vc;
    pipeline               = rc::Config::PipelineIds::Text;
    containsTransparency   = true;

    if (getSceneRef().object) {
        syncDrawParamsToScene();
        if (wasTrans != containsTransparency) { rebucket(); }
    }
}

} // namespace com
} // namespace bl
