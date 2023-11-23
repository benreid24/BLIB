#include <BLIB/Render/Overlays/OverlayObject.hpp>

namespace bl
{
namespace rc
{
namespace ovy
{
OverlayObject::OverlayObject()
: SceneObject()
, entity(ecs::InvalidEntity)
, overlay(nullptr)
, pipeline(nullptr)
, descriptorCount(0)
, perObjStart(0)
, overlayViewport(nullptr)
, cachedScissor{} {}

} // namespace ovy
} // namespace rc
} // namespace bl
