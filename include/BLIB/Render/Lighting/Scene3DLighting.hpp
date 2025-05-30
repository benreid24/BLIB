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
    static constexpr std::uint32_t MaxPointLights  = 128;
    static constexpr std::uint32_t MaxSpotLights   = 128;
    static constexpr std::uint32_t MaxPointShadows = 16;
    static constexpr std::uint32_t MaxSpotShadows  = 16;

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
     * @brief Creates a new spot light in the scene that casts shadows
     *
     * @tparam ...TArgs Argument types to the light's constructor
     * @param ... args Arguments to the light's constructor
     * @return A handle to the new light
     */
    template<typename... TArgs>
    SpotLightHandle createSpotlightWithShadow(TArgs&&... args);

    /**
     * @brief Directly returns a reference to the spotlight at the given index. May be invalidated
     *        so do not store
     *
     * @param i The index of the spotlight to access
     * @return A reference to the spotlight at the given index
     */
    SpotLight3D& getSpotlightUnsafe(std::uint32_t i);

    /**
     * @brief Returns the number of spotlights with shadows in the scene
     */
    std::uint32_t getSpotShadowCount() const;

    /**
     * @brief Returns the total number of spotlights in the scene
     */
    std::uint32_t getSpotLightCount() const;

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
     * @brief Creates a new point light in the scene that casts shadows
     *
     * @tparam ...TArgs Argument types to the light's constructor
     * @param ... Arguments to the light's constructor
     * @return A handle to the new light
     */
    template<typename... TArgs>
    PointLightHandle createPointLightWithShadow(TArgs&&... args);

    /**
     * @brief Directly returns a reference to the point light at the given index. May be invalidated
     *        so do not store
     *
     * @param i The index of the point light to access
     * @return A reference to the point light at the given index
     */
    PointLight3D& getPointLightUnsafe(std::uint32_t i);

    /**
     * @brief Returns the number of point lights with shadows in the scene
     */
    std::uint32_t getPointShadowCount() const;

    /**
     * @brief Returns the total number of point lights in the scene
     */
    std::uint32_t getPointLightCount() const;

    /**
     * @brief Set the position of the sun. Used for shadow map rendering
     *
     * @param position The position of the sun in world coordinates
     */
    void setSunPosition(const glm::vec3& position);

    /**
     * @brief Returns the position of the sun in world coordinates
     */
    const glm::vec3& getSunPosition() const;

    /**
     * @brief Called by owning scene prior to render. Do not call manually
     */
    void sync();

private:
    ds::Scene3DInstance& instance;
    glm::vec3 sunPosition;
    util::IdAllocator<std::size_t> spotIds;
    util::IdAllocator<std::size_t> spotShadowIds;
    std::vector<SpotLight3D> spotLights;
    std::vector<std::uint32_t> activeSpots;
    util::IdAllocator<std::size_t> pointIds;
    util::IdAllocator<std::size_t> pointShadowIds;
    std::vector<PointLight3D> pointLights;
    std::vector<std::uint32_t> activePoints;
    std::uint32_t spotShadowCount;
    std::uint32_t pointShadowCount;

    void removeLight(const PointLightHandle& light);
    void removeLight(const SpotLightHandle& light);
    void addIndex(std::vector<std::uint32_t>& vec, std::uint32_t i);

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
        return SpotLightHandle{
            this, spotLights, util::Random::get<std::size_t>(MaxSpotShadows, spotLights.size())};
    }

    const std::size_t i = spotIds.allocate() + MaxSpotShadows;
    new (&spotLights[i]) SpotLight3D(std::forward<TArgs>(args)...);
    addIndex(activeSpots, i);
    return SpotLightHandle{this, spotLights, i};
}

template<typename... TArgs>
SpotLightHandle Scene3DLighting::createSpotlightWithShadow(TArgs&&... args) {
    if (!spotShadowIds.available()) {
        BL_LOG_ERROR << "Exceeded max shadow spot light count";
        return SpotLightHandle{this, spotLights, util::Random::get<std::size_t>(0, MaxSpotShadows)};
    }

    const std::size_t i = spotShadowIds.allocate();
    new (&spotLights[i]) SpotLight3D(std::forward<TArgs>(args)...);
    addIndex(activeSpots, i);
    ++spotShadowCount;
    return SpotLightHandle{this, spotLights, i};
}

template<typename... TArgs>
PointLightHandle Scene3DLighting::createPointLight(TArgs&&... args) {
    if (!pointIds.available()) {
        BL_LOG_ERROR << "Exceeded max point light count";
        return PointLightHandle{
            this, pointLights, util::Random::get<std::size_t>(MaxPointShadows, pointLights.size())};
    }

    const std::size_t i = pointIds.allocate() + MaxPointShadows;
    new (&pointLights[i]) PointLight3D(std::forward<TArgs>(args)...);
    addIndex(activePoints, i);
    return PointLightHandle{this, pointLights, i};
}

template<typename... TArgs>
PointLightHandle Scene3DLighting::createPointLightWithShadow(TArgs&&... args) {
    if (!pointShadowIds.available()) {
        BL_LOG_ERROR << "Exceeded max shadow point light count";
        return PointLightHandle{
            this, pointLights, util::Random::get<std::size_t>(0, MaxPointShadows)};
    }

    const std::size_t i = pointShadowIds.allocate();
    new (&pointLights[i]) PointLight3D(std::forward<TArgs>(args)...);
    addIndex(activePoints, i);
    ++pointShadowCount;
    return PointLightHandle{this, pointLights, i};
}

inline const glm::vec3& Scene3DLighting::getSunPosition() const { return sunPosition; }

inline SpotLight3D& Scene3DLighting::getSpotlightUnsafe(std::uint32_t i) {
    return spotLights[activeSpots[i]];
}

inline std::uint32_t Scene3DLighting::getSpotShadowCount() const { return spotShadowCount; }

inline std::uint32_t Scene3DLighting::getSpotLightCount() const { return activeSpots.size(); }

inline PointLight3D& Scene3DLighting::getPointLightUnsafe(std::uint32_t i) {
    return pointLights[activePoints[i]];
}

inline std::uint32_t Scene3DLighting::getPointShadowCount() const { return pointShadowCount; }

inline std::uint32_t Scene3DLighting::getPointLightCount() const { return activePoints.size(); }

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
