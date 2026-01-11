#include <BLIB/Render/Lighting/Scene3DLighting.hpp>

#include <BLIB/Render/Config/Constants.hpp>
#include <algorithm>

namespace bl
{
namespace rc
{
namespace lgt
{
Scene3DLighting::Scene3DLighting(LightingDescriptor3D& uniform)
: uniform(uniform)
, spotLights(cfg::Limits::MaxSpotLights)
, spotShadows(cfg::Limits::MaxSpotShadows)
, pointLights(cfg::Limits::MaxPointLights)
, pointShadows(cfg::Limits::MaxPointShadows)
, sceneCenter(0.f, 0.f, 0.f)
, sunDistance(10.f) {
    uniform = LightingDescriptor3D();
}

glm::vec3 Scene3DLighting::getAmbientLightColor() const { return uniform.globalAmbient; }

void Scene3DLighting::setAmbientLightColor(const glm::vec3& c) { uniform.globalAmbient = c; }

const SunLight3D& Scene3DLighting::getSun() const { return uniform.sun; }

SunLight3D& Scene3DLighting::modifySun() { return uniform.sun; }

void Scene3DLighting::removeLight(const PointLightHandle& l) { pointLights.remove(l); }

void Scene3DLighting::removeLight(const PointLightShadowHandle& l) { pointShadows.remove(l); }

void Scene3DLighting::removeLight(const SpotLightHandle& l) { spotLights.remove(l); }

void Scene3DLighting::removeLight(const SpotLightShadowHandle& l) { spotShadows.remove(l); }

void Scene3DLighting::sync() {
    // TODO - consider allowing more lights then only copying a subset via culling
    updateSunCameraMatrix();
    uniform.nPointLights  = getPointLightCount();
    uniform.nSpotLights   = getSpotLightCount();
    uniform.nSpotShadows  = getSpotShadowCount();
    uniform.nPointShadows = getPointShadowCount();

    spotLights.copyToUniformBuffer(uniform.spotlights.data());
    spotShadows.copyToUniformBuffer(uniform.spotlightsWithShadows.data());
    pointLights.copyToUniformBuffer(uniform.pointLights.data());
    pointShadows.copyToUniformBuffer(uniform.pointLightsWithShadows.data());
}

void Scene3DLighting::addIndex(std::vector<std::uint32_t>& vec, std::uint32_t i) {
    vec.insert(std::lower_bound(vec.begin(), vec.end(), i), i);
}

void Scene3DLighting::setSceneCenter(const glm::vec3& c) { sceneCenter = c; }

void Scene3DLighting::setSunDistance(float d) { sunDistance = d; }

void Scene3DLighting::updateSunCameraMatrix() {
    // TODO - set matrices dynamically from camera (or cascaded shadow maps)
    const float r        = sunDistance * 0.5f;
    glm::mat4 projection = glm::ortho(-r, r, -r, r, 0.1f, sunDistance * 2.f);
    glm::mat4 view       = glm::lookAt(
        sceneCenter - uniform.sun.dir * sunDistance, sceneCenter, cfg::Constants::UpDirection);
    uniform.sun.viewProjectionMatrix = projection * view;
}

} // namespace lgt
} // namespace rc
} // namespace bl
