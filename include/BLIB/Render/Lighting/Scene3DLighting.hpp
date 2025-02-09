#ifndef BLIB_RENDER_LIGHTING_SCENE3DLIGHTING_HPP
#define BLIB_RENDER_LIGHTING_SCENE3DLIGHTING_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene3DInstance.hpp>
#include <BLIB/Render/Lighting/Light3D.hpp>
#include <BLIB/Render/Lighting/PointLight3D.hpp>
#include <BLIB/Render/Lighting/SpotLight3D.hpp>
#include <BLIB/Render/Lighting/SunLight3D.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <BLIB/Util/Random.hpp>

namespace bl
{
namespace rc
{
namespace lgt
{
/**
 * @brief Lighting manager for 3d scenes
 *
 * @ingroup Renderer
 */
class Scene3DLighting {
public:
    static constexpr std::uint32_t MaxPointLights = 128;
    static constexpr std::uint32_t MaxSpotLights  = 128;

    /**
     * @brief Creates the lighting manager
     *
     * @param instance The descriptor set instance for the scene
     */
    Scene3DLighting(ds::Scene3DInstance& instance);

    /**
     * @brief Returns the global ambient light color
     */
    glm::vec3 getAmbientLightColor() const;

    /**
     * @brief Sets the global ambient light color
     *
     * @param color The new global ambient light color
     */
    void setAmbientLightColor(const glm::vec3& color);

    /**
     * @brief Returns the sun light
     */
    const SunLight3D& getSun() const;

    /**
     * @brief Returns the sun light for modification
     */
    SunLight3D& modifySun();

    /**
     * @brief Creates a new spot light in the scene
     *
     * @tparam ...TArgs Argument types to the light's constructor
     * @param ... args Arguments to the light's constructor
     * @return A handle to the new light
     */
    template<typename... TArgs>
    SpotLightHandle createSpotlight(TArgs&&... args);

    /**
     * @brief Creates a new point light in the scene
     *
     * @tparam ...TArgs Argument types to the light's constructor
     * @param ... Arguments to the light's constructor
     * @return A handle to the new light
     */
    template<typename... TArgs>
    PointLightHandle createPointLight(TArgs&&... args);

    /**
     * @brief Called by owning scene prior to render. Do not call manually
     */
    void sync();

private:
    ds::Scene3DInstance& instance;
    util::IdAllocator<std::size_t> spotIds;
    std::vector<SpotLight3D> spotLights;
    std::vector<std::uint32_t> activeSpots;
    util::IdAllocator<std::size_t> pointIds;
    std::vector<PointLight3D> pointLights;
    std::vector<std::uint32_t> activePoints;

    void removeLight(const PointLightHandle& light);
    void removeLight(const SpotLightHandle& light);

    template<typename T>
    friend class Light3D;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename T>
void Light3D<T>::removeFromScene() {
    owner->removeLight(*this);
}

template<typename... TArgs>
SpotLightHandle Scene3DLighting::createSpotlight(TArgs&&... args) {
    if (!spotIds.available()) {
        BL_LOG_ERROR << "Exceeded max spot light count";
        return {*this, spotLights, util::Random::get<std::size_t>(0, spotLights.size())};
    }

    const std::size_t i = spotIds.allocate();
    new (&spotLights[i]) PointLight3D(std::forward<TArgs>(args)...);
    activeSpots.emplace_back(i);
    return {*this, spotLights, i};
}

template<typename... TArgs>
PointLightHandle Scene3DLighting::createPointLight(TArgs&&... args) {
    if (!spotIds.available()) {
        BL_LOG_ERROR << "Exceeded max point light count";
        return {*this, pointLights, util::Random::get<std::size_t>(0, pointLights.size())};
    }

    const std::size_t i = pointIds.allocate();
    new (&pointLights[i]) PointLight3D(std::forward<TArgs>(args)...);
    activePoints.emplace_back(i);
    return {*this, pointLights, i};
}

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
