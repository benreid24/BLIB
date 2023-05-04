#ifndef BLIB_RENDER_CONFIG_HPP
#define BLIB_RENDER_CONFIG_HPP

#include <BLIB/Logging.hpp>
#include <cstdint>
#include <glm/glm.hpp>

namespace bl
{
namespace render
{
struct Config {
    static constexpr unsigned int MaxConcurrentFrames = 2;

    static constexpr unsigned int MaxSceneObservers = 5;

    static constexpr glm::vec3 UpDirection = {0.f, 1.f, 0.f};

    struct SceneObjectStage { // does not include postfx stage as objects do not belong in there
        enum : std::size_t { ShadowMapping = 0, OpaquePass = 1, TransparentPass = 2, Count };
    };

    struct ShaderIds {
        static constexpr char OpaqueVertexShader[]    = {0};
        static constexpr char OpaqueFragmentShader[]  = {1};
        static constexpr char SkinnedVertexShader[]   = {2};
        static constexpr char SkinnedFragmentShader[] = {3};
        static constexpr char EmptyVertex[]           = {4};
        static constexpr char DefaultPostFXFragment[] = {5};

        static constexpr std::uint32_t MaxId = 31;
    };

    struct PipelineIds {
        static constexpr std::uint32_t None = 0;

        static constexpr std::uint32_t ShadowPassMeshes        = 1;
        static constexpr std::uint32_t ShadowPassSkinnedMeshes = 2;

        static constexpr std::uint32_t OpaqueMeshes        = 3;
        static constexpr std::uint32_t OpaqueSkinnedMeshes = 4;

        static constexpr std::uint32_t TransparentSkinnedMeshes = 5;

        static constexpr std::uint32_t PostFXBase = 6;
    };

    struct RenderPassIds {
        static constexpr std::uint32_t OffScreenSceneRender   = 0;
        static constexpr std::uint32_t SwapchainPrimaryRender = 1;
    };
};

} // namespace render
} // namespace bl

#endif
