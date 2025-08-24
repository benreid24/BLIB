#ifndef BLIB_RENDER_LIGHTING_SCENE2DLIGHTING_HPP
#define BLIB_RENDER_LIGHTING_SCENE2DLIGHTING_HPP

#include <BLIB/Render/Descriptors/Builtin/Scene2DInstance.hpp>
#include <BLIB/Render/Lighting/Light2D.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <array>
#include <cstdint>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace scene
{
class Scene2D;
}

/// Contains the lighting classes for the renderer
namespace lgt
{
/**
 * @brief Lighting manager for 2d scenes
 *
 * @ingroup Renderer
 */
class Scene2DLighting {
public:
    static constexpr std::uint32_t MaxLightCount = dsi::Scene2DInstance::MaxLightCount;

    /**
     * @brief Creates the lighting manager
     *
     * @param descriptorSet The descriptor set containing the light buffers
     */
    Scene2DLighting(dsi::Scene2DInstance* descriptorSet);

    /**
     * @brief Sets the ambient light color. Default is white (full light)
     *
     * @param ambient The ambient light color
     */
    void setAmbientLight(const glm::vec3& ambient);

    /**
     * @brief Returns the current ambient light color
     */
    const glm::vec3& getAmbientLight() const;

    /**
     * @brief Adds a light to the scene
     *
     * @param position The position of the light in world coordinates
     * @param radius The radius of the light in world units
     * @param color The color of the light
     * @return A handle to manage the newly created light
     */
    Light2D addLight(const glm::vec2& position, float radius,
                     const glm::vec3& color = glm::vec3(1.f));

    /**
     * @brief Removes the given light from the scene
     *
     * @param light The light to remove
     */
    void removeLight(Light2D light);

    /**
     * @brief Removes all lights from the scene
     */
    void removeAllLights();

    /**
     * @brief Returns a light handle for the given light. Does not check validity
     *
     * @param id The id of the light to construct a handle for
     * @return A handle to the light
     */
    Light2D getLight(std::uint32_t id) { return Light2D(*this, id); }

private:
    dsi::Scene2DInstance::Lighting& lighting;
    util::IdAllocator<std::uint32_t> indexAllocator;
    std::array<std::uint32_t, MaxLightCount> allocations;

    friend class scene::Scene2D;
    friend class Light2D;
};

} // namespace lgt
} // namespace rc
} // namespace bl

#endif
