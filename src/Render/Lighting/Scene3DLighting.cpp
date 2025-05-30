#include <BLIB/Render/Lighting/Scene3DLighting.hpp>

#include <algorithm>

namespace bl
{
namespace rc
{
namespace lgt
{
namespace
{
constexpr std::uint32_t SpotlightCount =
    Scene3DLighting::MaxSpotLights - Scene3DLighting::MaxSpotShadows;
constexpr std::uint32_t PointLightCount =
    Scene3DLighting::MaxPointLights - Scene3DLighting::MaxPointShadows;
} // namespace

Scene3DLighting::Scene3DLighting(ds::Scene3DInstance& instance)
: instance(instance)
, spotIds(SpotlightCount)
, spotShadowIds(MaxPointShadows)
, spotLights(MaxSpotLights, SpotLight3D())
, pointIds(PointLightCount)
, pointShadowIds(MaxPointShadows)
, pointLights(MaxPointLights, PointLight3D())
, spotShadowCount(0)
, pointShadowCount(0) {
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
    if (i < MaxPointShadows) {
        --pointShadowCount;
        pointShadowIds.release(i);
    }
    else { pointIds.release(i - MaxPointShadows); }
    for (auto it = activePoints.begin(); it != activePoints.end(); ++it) {
        if (*it == i) {
            activePoints.erase(it);
            break;
        }
    }
}

void Scene3DLighting::removeLight(const SpotLightHandle& l) {
    const std::size_t i = &l.get() - spotLights.data();
    if (i < MaxSpotShadows) {
        --spotShadowCount;
        spotShadowIds.release(i);
    }
    else { spotIds.release(i - MaxPointShadows); }
    for (auto it = activeSpots.begin(); it != activeSpots.end(); ++it) {
        if (*it == i) {
            activeSpots.erase(it);
            break;
        }
    }
}

void Scene3DLighting::sync() {
    // TODO - consider allowing more lights then only copying a subset via culling
    instance.globalLightInfo[0].nSpotShadows  = spotShadowCount;
    instance.globalLightInfo[0].nPointShadows = pointShadowCount;

    std::size_t di                           = 0;
    instance.globalLightInfo[0].nPointLights = activePoints.size() - pointShadowCount;
    for (std::size_t i : activePoints) { instance.pointLights[di++] = pointLights[i]; }

    di                                      = 0;
    instance.globalLightInfo[0].nSpotLights = activeSpots.size() - spotShadowCount;
    for (std::size_t i : activeSpots) {
        auto& light       = instance.spotlights[di++];
        light             = spotLights[i];
        light.cutoff      = std::cos(glm::radians(light.cutoff));
        light.outerCutoff = std::cos(glm::radians(light.outerCutoff));
    }
}

void Scene3DLighting::addIndex(std::vector<std::uint32_t>& vec, std::uint32_t i) {
    vec.insert(std::lower_bound(vec.begin(), vec.end(), i), i);
}

void Scene3DLighting::setSunPosition(const glm::vec3& sp) { sunPosition = sp; }

} // namespace lgt
} // namespace rc
} // namespace bl
