#ifndef BLIB_RENDER_CONFIG_HPP
#define BLIB_RENDER_CONFIG_HPP

#include <glm/glm.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Render/Uniforms/PushConstants.hpp>
#include <cstdint>

namespace bl
{
namespace render
{
struct Config {
    static constexpr unsigned int MaxConcurrentFrames = 2;

    static constexpr std::uint32_t CustomPushConstantOffsetStart  = sizeof(PushConstants);
    static constexpr std::uint32_t CustomPushConstantMinAvailSize = 128 - sizeof(PushConstants);

    static constexpr glm::vec3 UpDirection = {0.f, 1.f, 0.f};

    struct Stage { // does not include postfx stage as objects do not belong in there
        enum : std::size_t {
            ShadowMapping      = 0,
            PrimaryOpaque      = 1,
            PrimaryTransparent = 2,
            SceneOverlay       = 3,
            ObserverOverlay    = 4,
            TopOverlay         = 5,
            Count
        };
    };

    struct ShaderIds {
        static constexpr char TestVertexShader[]   = {0};
        static constexpr char TestFragmentShader[] = {1};

        static constexpr std::uint32_t MaxId = 31;
    };

    struct PipelineIds {
        static constexpr std::uint32_t None = 0;

        static constexpr std::uint32_t ShadowPassMeshes        = 1;
        static constexpr std::uint32_t ShadowPassSkinnedMeshes = 2;

        static constexpr std::uint32_t OpaqueMeshes        = 3;
        static constexpr std::uint32_t OpaqueSkinnedMeshes = 4;

        static constexpr std::uint32_t TransparentSkinnedMeshes = 5;
    };

    struct RenderPassIds {
        static constexpr std::uint32_t PrimaryScene = 0;
    };
};

} // namespace render
} // namespace bl

#endif
