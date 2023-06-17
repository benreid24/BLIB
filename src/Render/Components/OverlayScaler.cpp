#include <BLIB/Render/Components/OverlayScaler.hpp>

namespace bl
{
namespace render
{
namespace com
{
OverlayScaler::OverlayScaler()
: cachedTargetSize(1920.f, 1080.f)
, cachedOverlaySize(1.f, 1.f)
, cachedObjectSize(50.f, 50.f)
, scaleType(None)
, sizePercent(0.1f, 0.1f)
, dirty(false) {}

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

void OverlayScaler::setTargetSize(const glm::vec2& s) {
    cachedTargetSize = s;
    dirty            = scaleType != None;
}

} // namespace com
} // namespace render
} // namespace bl
