#include <BLIB/Render/Lighting/Scene3DLighting.hpp>

#include <algorithm>

namespace bl
{
namespace rc
{
namespace lgt
{
Scene3DLighting::Scene3DLighting(ds::Scene3DInstance& instance)
: instance(instance)
, spotLights(Config::MaxSpotLights)
, spotShadows(Config::MaxSpotShadows)
, pointLights(Config::MaxPointLights)
, pointShadows(Config::MaxPointShadows)
, sceneCenter(0.f, 0.f, 0.f)
, sunDistance(10.f) {
    instance.getUniform() = LightingDescriptor3D();
}

glm::vec3 Scene3DLighting::getAmbientLightColor() const {
    return instance.getUniform().globalAmbient;
}

void Scene3DLighting::setAmbientLightColor(const glm::vec3& c) {
    instance.getUniform().globalAmbient = c;
}

const SunLight3D& Scene3DLighting::getSun() const { return instance.getUniform().sun; }

SunLight3D& Scene3DLighting::modifySun() { return instance.getUniform().sun; }

void Scene3DLighting::removeLight(const PointLightHandle& l) { pointLights.remove(l); }

void Scene3DLighting::removeLight(const PointLightShadowHandle& l) { pointShadows.remove(l); }

void Scene3DLighting::removeLight(const SpotLightHandle& l) { spotLights.remove(l); }

void Scene3DLighting::removeLight(const SpotLightShadowHandle& l) { spotShadows.remove(l); }

void Scene3DLighting::sync() {
    // TODO - consider allowing more lights then only copying a subset via culling
    updateSunCameraMatrix();
    instance.getUniform().nPointLights  = getPointLightCount();
    instance.getUniform().nSpotLights   = getSpotLightCount();
    instance.getUniform().nSpotShadows  = getSpotShadowCount();
    instance.getUniform().nPointShadows = getPointShadowCount();

    spotLights.copyToUniformBuffer(instance.getUniform().spotlights.data());
    spotShadows.copyToUniformBuffer(instance.getUniform().spotlightsWithShadows.data());
    pointLights.copyToUniformBuffer(instance.getUniform().pointLights.data());
    pointShadows.copyToUniformBuffer(instance.getUniform().pointLightsWithShadows.data());
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
    glm::mat4 view       = glm::lookAt(sceneCenter - instance.getUniform().sun.dir * sunDistance,
                                 sceneCenter,
                                 Config::UpDirection);
    instance.getUniform().sun.viewProjectionMatrix = projection * view;
}

} // namespace lgt
} // namespace rc
} // namespace bl
