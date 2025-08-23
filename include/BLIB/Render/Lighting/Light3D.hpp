#ifndef BLIB_RENDER_LIGHTING_LIGHT3D_HPP
#define BLIB_RENDER_LIGHTING_LIGHT3D_HPP

#include <BLIB/Render/Lighting/PointLight3D.hpp>
#include <BLIB/Render/Lighting/PointLight3DShadow.hpp>
#include <BLIB/Render/Lighting/SpotLight3D.hpp>
#include <BLIB/Render/Lighting/SpotLight3DShadow.hpp>
#include <BLIB/Util/VectorRef.hpp>

namespace bl
{
namespace rc
{
namespace lgt
{
class Scene3DLighting;

/**
 * @brief Handle to a light in 3d scene
 *
 * @tparam T The type of light being managed
 * @ingroup Renderer
 */
template<typename T>
class Light3D {
public:
    /**
     * @brief Copy the light handle
     */
    Light3D(const Light3D&) = default;

    /**
     * @brief Copy the light handle
     */
    Light3D(Light3D&&) = default;

    /**
     * @brief Copy the light handle
     */
    Light3D& operator=(const Light3D&) = default;

    /**
     * @brief Copy the light handle
     */
    Light3D& operator=(Light3D&&) = default;

    /**
     * @brief Returns the value of the light
     */
    const T& get() const { return *light; }

    /**
     * @brief Returns the light to be modified and marks it for copy
     * @return
     */
    T& get() { return *light; }

    /**
     * @brief Removes the light from the scene
     */
    void removeFromScene();

private:
    Scene3DLighting* owner;
    util::VectorRef<T> light;
    glm::vec3 color;
    float brightness;
    float specularFactor;
    float diffuseFactor;
    float ambientFactor;

    Light3D(Scene3DLighting* owner, std::vector<T>& v, std::size_t i)
    : owner(owner)
    , light(v, i) {}

    friend class Scene3DLighting;
};

/**
 * @brief Handle to a point light in 3d scene
 *
 * @ingroup Renderer
 */
using PointLightHandle = Light3D<PointLight3D>;

/**
 * @brief Handle to a point light with shadow in 3d scene
 *
 * @ingroup Renderer
 */
using PointLightShadowHandle = Light3D<PointLight3DShadow>;

/**
 * @brief Handle to a spot light in 3d scene
 *
 * @ingroup Renderer
 */
using SpotLightHandle = Light3D<SpotLight3D>;

/**
 * @brief Handle to a spot light with shadow in 3d scene
 *
 * @ingroup Renderer
 */
using SpotLightShadowHandle = Light3D<SpotLight3DShadow>;

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
