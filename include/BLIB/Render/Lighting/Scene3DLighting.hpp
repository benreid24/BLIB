#ifndef BLIB_RENDER_LIGHTING_SCENE3DLIGHTING_HPP
#define BLIB_RENDER_LIGHTING_SCENE3DLIGHTING_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Lighting/Light3D.hpp>
#include <BLIB/Render/Lighting/LightingDescriptor3D.hpp>
#include <BLIB/Render/Lighting/PointLight3D.hpp>
#include <BLIB/Render/Lighting/PointLight3DShadow.hpp>
#include <BLIB/Render/Lighting/SpotLight3D.hpp>
#include <BLIB/Render/Lighting/SpotLight3DShadow.hpp>
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
    /**
     * @brief Creates the lighting manager
     *
     * @param uniform The lighting uniform to update with light data
     */
    Scene3DLighting(LightingDescriptor3D& uniform);

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
    SpotLightShadowHandle createSpotlightWithShadow(TArgs&&... args);

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
    PointLightShadowHandle createPointLightWithShadow(TArgs&&... args);

    /**
     * @brief Returns the number of point lights with shadows in the scene
     */
    std::uint32_t getPointShadowCount() const;

    /**
     * @brief Returns the total number of point lights in the scene
     */
    std::uint32_t getPointLightCount() const;

    /**
     * @brief Sets the center of the scene in world coordinates. Used for sun shadows
     *
     * @param center The center of the scene in world coordinates
     */
    void setSceneCenter(const glm::vec3& center);

    /**
     * @brief Returns the center of the scene in world coordinates
     */
    const glm::vec3& getSceneCenter() const;

    /**
     * @brief Sets the distance of the sun from the scene center
     *
     * @param distance The distance of the sun from the scene center in world units
     */
    void setSunDistance(float distance);

    /**
     * @brief Returns the distance of the sun from the scene center
     */
    float getSunDistance() const;

    /**
     * @brief Updates the view projection matrix for the sunlight camera
     */
    void updateSunCameraMatrix();

    /**
     * @brief Called by owning scene prior to render. Do not call manually
     */
    void sync();

private:
    template<typename T>
    struct Lights {
        util::IdAllocator<std::size_t> idAllocator;
        std::vector<T> lights;
        std::vector<std::uint32_t> active;

        Lights(std::size_t maxCount)
        : idAllocator(maxCount) {
            lights.resize(maxCount);
            active.reserve(maxCount);
        }

        template<typename... TArgs>
        Light3D<T> createNew(Scene3DLighting* owner, TArgs&&... args) {
            if (!idAllocator.available()) {
                BL_LOG_ERROR << "Exceeded max light count";
                return {owner, lights, util::Random::get<std::size_t>(0, lights.size() - 1)};
            }
            const std::size_t i = idAllocator.allocate();
            new (&lights[i]) T(std::forward<TArgs>(args)...);
            active.emplace_back(i);
            return {owner, lights, i};
        }

        void remove(const Light3D<T>& light) {
            const std::size_t i = &light.get() - lights.data();
            if (i < lights.size()) {
                idAllocator.release(i);
                for (auto it = active.begin(); it != active.end(); ++it) {
                    if (*it == i) {
                        active.erase(it);
                        break;
                    }
                }
            }
        }

        void copyToUniformBuffer(T* dst) {
            std::uint32_t ui = 0;
            for (std::uint32_t i : active) { dst[ui++].copyAsUniform(lights[i]); }
        }
    };

    LightingDescriptor3D& uniform;
    glm::vec3 sceneCenter;
    float sunDistance;
    Lights<SpotLight3D> spotLights;
    Lights<SpotLight3DShadow> spotShadows;
    Lights<PointLight3D> pointLights;
    Lights<PointLight3DShadow> pointShadows;

    void removeLight(const PointLightHandle& light);
    void removeLight(const PointLightShadowHandle& light);
    void removeLight(const SpotLightHandle& light);
    void removeLight(const SpotLightShadowHandle& light);
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
    return spotLights.createNew(this, std::forward<TArgs>(args)...);
}

template<typename... TArgs>
SpotLightShadowHandle Scene3DLighting::createSpotlightWithShadow(TArgs&&... args) {
    return spotShadows.createNew(this, std::forward<TArgs>(args)...);
}

template<typename... TArgs>
PointLightHandle Scene3DLighting::createPointLight(TArgs&&... args) {
    return pointLights.createNew(this, std::forward<TArgs>(args)...);
}

template<typename... TArgs>
PointLightShadowHandle Scene3DLighting::createPointLightWithShadow(TArgs&&... args) {
    return pointShadows.createNew(this, std::forward<TArgs>(args)...);
}

inline const glm::vec3& Scene3DLighting::getSceneCenter() const { return sceneCenter; }

inline float Scene3DLighting::getSunDistance() const { return sunDistance; }

inline std::uint32_t Scene3DLighting::getSpotShadowCount() const {
    return spotShadows.active.size();
}

inline std::uint32_t Scene3DLighting::getSpotLightCount() const { return spotLights.active.size(); }

inline std::uint32_t Scene3DLighting::getPointShadowCount() const {
    return pointShadows.active.size();
}

inline std::uint32_t Scene3DLighting::getPointLightCount() const {
    return pointLights.active.size();
}

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
