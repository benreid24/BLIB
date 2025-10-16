#ifndef BLIB_RENDER_LIGHTING_SCENE2DLIGHTING_HPP
#define BLIB_RENDER_LIGHTING_SCENE2DLIGHTING_HPP

#include <BLIB/Render/Buffers/StaticUniformBuffer.hpp>
#include <BLIB/Render/Lighting/Light2D.hpp>
#include <BLIB/Render/ShaderResources/BufferShaderResource.hpp>
#include <BLIB/Render/ShaderResources/Key.hpp>
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
    static constexpr std::uint32_t MaxLightCount = 500;

    /**
     * @brief POD of a 2d light
     */
    struct alignas(16) Light {
        glm::vec4 color;
        glm::vec2 position;
    };

    /**
     * @brief POD for 2d scene lighting data
     */
    struct alignas(16) Lighting {
        std::uint32_t lightCount;
        glm::vec3 ambient;
        alignas(16) Light lights[MaxLightCount];
    };

    /**
     * @brief Creates the lighting manager
     *
     * @param lightingBuffer The lighting buffer to update with light data
     */
    Scene2DLighting(Lighting& lightingBuffer);

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
    Lighting& lighting;
    util::IdAllocator<std::uint32_t> indexAllocator;
    std::array<std::uint32_t, MaxLightCount> allocations;

    friend class scene::Scene2D;
    friend class Light2D;
};

} // namespace lgt

namespace sri
{
/**
 * @brief Buffer shader resource containing 2d scene lighting data
 *
 * @ingroup Renderer
 */
using LightingBuffer2D =
    sr::BufferShaderResource<buf::StaticUniformBuffer<lgt::Scene2DLighting::Lighting>, 1>;

constexpr sr::Key<LightingBuffer2D> Scene2DLightingKey("__builtin_Scene2DLighting");
} // namespace sri

} // namespace rc
} // namespace bl

#endif
