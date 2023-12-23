#include <BLIB/Render/Lighting/Scene2DLighting.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace rc
{
namespace lgt
{
Scene2DLighting::Scene2DLighting(ds::Scene2DInstance* desc)
: lighting(desc->lighting[0])
, indexAllocator(MaxLightCount) {
    allocations.fill(MaxLightCount);
}

void Scene2DLighting::setAmbientLight(const glm::vec3& ambient) { lighting.ambient = ambient; }

const glm::vec3& Scene2DLighting::getAmbientLight() const { return lighting.ambient; }

Light2D Scene2DLighting::addLight(const glm::vec2& position, float radius, const glm::vec3& color) {
    const std::uint32_t i       = indexAllocator.allocate();
    const std::uint32_t j       = lighting.lightCount++;
    allocations[i]              = j;
    lighting.lights[j].color    = glm::vec4(color, radius);
    lighting.lights[j].position = position;
    return Light2D(*this, i);
}

void Scene2DLighting::removeLight(Light2D light) {
    if (light.owner != this) {
        BL_LOG_WARN << "Cannot remove light from a different scene";
        return;
    }

    const std::uint32_t i = allocations[light.index];
    if (i >= MaxLightCount) { return; }

    if (lighting.lightCount > 1) {
        const std::uint32_t j = lighting.lightCount - 1;
        lighting.lights[i]    = lighting.lights[j];
        for (std::uint32_t& alloc : allocations) {
            if (alloc == j) {
                alloc = i;
                break;
            }
        }
    }

    --lighting.lightCount;
    indexAllocator.release(light.index);
    allocations[i] = MaxLightCount;
}

void Scene2DLighting::removeAllLights() {
    lighting.lightCount = 0;
    indexAllocator.releaseAll();
    allocations.fill(MaxLightCount);
}

} // namespace lgt
} // namespace rc
} // namespace bl
