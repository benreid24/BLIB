#ifndef BLIB_RENDER_CONFIG_HPP
#define BLIB_RENDER_CONFIG_HPP

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

    struct RenderPassIds {
        static constexpr std::uint32_t Shadow      = 0;
        static constexpr std::uint32_t Opaque      = 1;
        static constexpr std::uint32_t Transparent = 2;
        static constexpr std::uint32_t PostFX      = 3;
        static constexpr std::uint32_t Overlay     = 4;
        static constexpr std::uint32_t Count       = 5;
    };

    struct RenderPassBits {
        static constexpr std::uint32_t None        = 0;
        static constexpr std::uint32_t Shadow      = 0x1 << 0;
        static constexpr std::uint32_t Opaque      = 0x1 << 1;
        static constexpr std::uint32_t Transparent = 0x1 << 2;
        static constexpr std::uint32_t PostFX      = 0x1 << 3;
        static constexpr std::uint32_t Overlay     = 0x1 << 4;

        static constexpr std::uint32_t bitFromId(std::uint32_t id) {
#ifdef BLIB_DEBUG
            if (id >= RenderPassIds::Count) { BL_LOG_CRITICAL << "Invalid render pass id: " << id; }
#endif
            return 0x1 << id;
        }
    };

    struct BuiltInShaderIds {
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
};

} // namespace render
} // namespace bl

#endif
