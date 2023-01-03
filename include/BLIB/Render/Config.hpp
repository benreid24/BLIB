#ifndef BLIB_RENDER_CONFIG_HPP
#define BLIB_RENDER_CONFIG_HPP

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
        static constexpr std::uint32_t GBuffer  = 0;
        static constexpr std::uint32_t Lighting = 1;
        static constexpr std::uint32_t PostFX   = 2;
        static constexpr std::uint32_t Overlay  = 3;
    };

    struct BuiltInshaderIds {
        static constexpr char TestVertexShader   = 0;
        static constexpr char TestFragmentShader = 1;

        static constexpr std::uint32_t MaxId = 31;
    };

    struct PipelineIds {
        static constexpr std::uint32_t TestingOnly = 0;
    };
};

} // namespace render
} // namespace bl

#endif
