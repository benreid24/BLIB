#include <BLIB/Render/Overlays/OverlayObject.hpp>

namespace bl
{
namespace rc
{
namespace ovy
{
OverlayObject::OverlayObject()
: SceneObject()
, overlay(nullptr)
, pipeline(nullptr)
, overlayViewport(nullptr)
, cachedScissor{} {}

} // namespace ovy
} // namespace rc
} // namespace bl
