#include <BLIB/Cameras/OverlayCamera.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace bl
{
namespace cam
{
namespace
{
constexpr const char* WidthKey  = "blib.overlay.width";
constexpr const char* HeightKey = "blib.overlay.height";

glm::vec2 overlaySpace(1.f, 1.f);
bool configLoaded          = false;
unsigned int configVersion = 0;

void ensureConfigLoaded() {
    if (!configLoaded) {
        configLoaded   = true;
        overlaySpace.x = engine::Configuration::getOrDefault<float>(WidthKey, 1.f);
        overlaySpace.y = engine::Configuration::getOrDefault<float>(HeightKey, 1.f);
    }
}
} // namespace

OverlayCamera::OverlayCamera()
: Camera()
, settingVersion(0) {
    markViewDirty();
    markProjDirty();
}

void OverlayCamera::update(float) {
    if (settingVersion != configVersion) {
        settingVersion = configVersion;
        markProjDirty();
    }
}

void OverlayCamera::refreshViewMatrix(glm::mat4& view) { view = glm::mat4(1.f); }

void OverlayCamera::refreshProjMatrix(glm::mat4& proj, const VkViewport&) {
    ensureConfigLoaded();
    proj = glm::ortho(0.f, overlaySpace.x, 0.f, overlaySpace.y, MinDepth, MaxDepth);
}

void OverlayCamera::setOverlayCoordinateSpace(float width, float height) {
    overlaySpace = {width, height};
    ++configVersion;
    engine::Configuration::set<float>(WidthKey, width);
    engine::Configuration::set<float>(HeightKey, height);
}

const glm::vec2& OverlayCamera::getOverlayCoordinateSpace() { return overlaySpace; }

} // namespace cam
} // namespace bl
