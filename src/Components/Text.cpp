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
    containsTransparency   = true;

    if (getSceneRef().object) {
        if (wasTrans != containsTransparency) { rebucket(); }
    }
}

} // namespace com
} // namespace bl
