#include <BLIB/Components/OverlayScaler.hpp>

namespace bl
{
namespace com
{
OverlayScaler::OverlayScaler()
: cachedObjectBounds(0.f, 0.f, 50.f, 50.f)
, scaleType(None)
, sizePercent(0.1f, 0.1f)
, posType(NoPosition)
, scissorMode(ScissorInherit)
, dirty(true) {}

void OverlayScaler::scaleToWidthPercent(float p) {
    scaleType    = WidthPercent;
    widthPercent = p;
    dirty        = true;
}

void OverlayScaler::scaleToHeightPercent(float p) {
    scaleType     = HeightPercent;
    heightPercent = p;
    dirty         = true;
}

void OverlayScaler::scaleToHeightRatio(float src, float dst) {
    scaleType    = LineHeight;
    overlayRatio = dst / src;
    dirty        = true;
}

void OverlayScaler::scaleToSizePercent(const glm::vec2& p) {
    scaleType   = SizePercent;
    sizePercent = p;
    dirty       = true;
}

void OverlayScaler::mapToTargetPixels(float r) {
    scaleType  = PixelRatio;
    pixelRatio = r;
    dirty      = true;
}

void OverlayScaler::stopScaling() {
    scaleType = None;
    dirty     = false;
}

void OverlayScaler::setEntityBounds(const sf::FloatRect& s) {
    cachedObjectBounds = s;
    dirty              = true;
}

void OverlayScaler::setScissorMode(ScissorMode m) {
    scissorMode = m;
    dirty       = true;
}

void OverlayScaler::positionInParentSpace(const glm::vec2& pos) {
    posType        = ParentSpace;
    parentPosition = pos;
    dirty          = true;
}

void OverlayScaler::stopPositioning() {
    dirty   = posType != NoPosition;
    posType = NoPosition;
}

void OverlayScaler::setScaleCallback(OnScale&& cb) { onScale = std::forward<OnScale>(cb); }

} // namespace com
} // namespace bl
