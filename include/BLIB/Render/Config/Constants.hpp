#ifndef BLIB_RENDER_CONFIG_CONSTANTS_HPP
#define BLIB_RENDER_CONFIG_CONSTANTS_HPP

#include <array>
#include <cstdint>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
/// Namespace containing constant configuration for the renderer
namespace cfg
{
/**
 * @brief Constants defined at compile time for the renderer
 *
 * @ingroup Renderer
 */
struct Constants {
    static constexpr std::uint32_t SPIRVMagicNumber = 0x07230203;

    static constexpr std::uint32_t DefaultSceneObjectCapacity = 128;

    static constexpr glm::vec3 UpDirection  = {0.f, 1.f, 0.f};
    static constexpr glm::vec3 Rotate2DAxis = {0.f, 0.f, 1.f};

    static constexpr std::array<glm::vec3, 6> CubemapDirections = {
        glm::vec3(1.f, 0.f, 0.f),  // Right
        glm::vec3(-1.f, 0.f, 0.f), // Left
        glm::vec3(0.f, 1.f, 0.f),  // Up
        glm::vec3(0.f, -1.f, 0.f), // Down
        glm::vec3(0.f, 0.f, 1.f),  // Back
        glm::vec3(0.f, 0.f, -1.f)  // Front
    };

    static constexpr std::array<glm::vec3, 6> CubeUpVectors = {
        -UpDirection,
        -UpDirection,
        {-1.f, 0.f, 0.f},
        {1.f, 0.f, 0.f},
        -UpDirection,
        -UpDirection,
    };

    static constexpr float DefaultSpecularLightFactor = 0.25f;
    static constexpr float DefaultDiffuseLightFactor  = 1.f;
    static constexpr float DefaultAmbientLightFactor  = 0.1f;
};

} // namespace cfg
} // namespace rc
} // namespace bl

#endif
