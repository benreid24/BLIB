#include <BLIB/Render/Lighting/Scene3DLighting.hpp>

namespace bl
{
namespace rc
{
namespace lgt
{
Scene3DLighting::Scene3DLighting(ds::Scene3DInstance& instance)
: instance(instance)
, spotIds(MaxSpotLights)
, spotLights(MaxSpotLights, SpotLight3D())
, pointIds(MaxPointLights)
, pointLights(MaxPointLights, PointLight3D()) {
    activeSpots.reserve(MaxSpotLights);
    activePoints.reserve(MaxPointLights);
}

glm::vec3 Scene3DLighting::getAmbientLightColor() const {
    return instance.globalLightInfo[0].globalAmbient;
}

void Scene3DLighting::setAmbientLightColor(const glm::vec3& c) {
    instance.globalLightInfo[0].globalAmbient = c;
}

const SunLight3D& Scene3DLighting::getSun() const { return instance.globalLightInfo[0].sun; }

SunLight3D& Scene3DLighting::modifySun() { return instance.globalLightInfo[0].sun; }

void Scene3DLighting::removeLight(const PointLightHandle& l) {
    const std::size_t i = &l.get() - pointLights.data();
    pointIds.release(i);
    for (auto it = activePoints.begin(); it != activePoints.end(); ++it) {
        if (*it == i) {
            activePoints.erase(it);
            break;
        }
    }
}

void Scene3DLighting::removeLight(const SpotLightHandle& l) {
    const std::size_t i = &l.get() - spotLights.data();
    spotIds.release(i);
    for (auto it = activeSpots.begin(); it != activeSpots.end(); ++it) {
        if (*it == i) {
            activeSpots.erase(it);
            break;
        }
    }
}

void Scene3DLighting::sync() {
    // TODO - consider allowing more lights then only copying a subset via culling

    std::size_t di                           = 0;
    instance.globalLightInfo[0].nPointLights = activePoints.size();
    for (std::size_t i : activePoints) { instance.pointLights[di++] = pointLights[i]; }

    di                                      = 0;
    instance.globalLightInfo[0].nSpotLights = activeSpots.size();
    for (std::size_t i : activeSpots) { instance.spotlights[di++] = spotLights[i]; }
}

} // namespace lgt
} // namespace rc
} // namespace bl
