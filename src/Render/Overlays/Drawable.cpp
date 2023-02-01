#include <BLIB/Render/Overlays/Drawable.hpp>

namespace bl
{
namespace render
{
namespace overlay
{
Drawable::Drawable()
: hidden(false) {}

void Drawable::setHidden(bool h) { hidden = h; }

void Drawable::render(OverlayRenderContext& ctx) {
    if (!hidden) { doRender(ctx); }
}

} // namespace overlay
} // namespace render
} // namespace bl
