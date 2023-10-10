#include <BLIB/Components/OverlayScaler.hpp>

namespace bl
{
namespace com
{
OverlayScaler::OverlayScaler()
: cachedObjectSize(50.f, 50.f)
, scaleType(None)
, sizePercent(0.1f, 0.1f)
, useScissor(false)
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

void OverlayScaler::setEntitySize(const glm::vec2& s) {
    cachedObjectSize = s;
    dirty            = scaleType != None;
}

void OverlayScaler::setScissorToSelf(bool s) {
    useScissor = s;
    dirty      = scaleType != None;
}

} // namespace com
} // namespace bl
