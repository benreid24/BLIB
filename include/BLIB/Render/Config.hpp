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

    static constexpr std::size_t MaxDescriptorSets = 4;

    static constexpr glm::vec3 UpDirection  = {0.f, 1.f, 0.f};
    static constexpr glm::vec3 Rotate2DAxis = {0.f, 0.f, 1.f};

    struct SceneObjectStage { // does not include postfx stage as objects do not belong in there
        enum : std::size_t { ShadowMapping = 0, OpaquePass = 1, TransparentPass = 2, Count };
    };

    struct ShaderIds {
        static constexpr char EmptyVertex[] = {0};

        static constexpr char MeshVertex[]          = {1};
        static constexpr char MeshFragment[]        = {2};
        static constexpr char SkinnedMeshVertex[]   = {3};
        static constexpr char SkinnedMeshFragment[] = {4};

        static constexpr char Lit2DVertex[]          = {5};
        static constexpr char Lit2DFragment[]        = {6};
        static constexpr char LitSkinned2DVertex[]   = {7};
        static constexpr char LitSkinned2DFragment[] = {8};

        static constexpr char Unlit2DVertex[]          = {9};
        static constexpr char Unlit2DFragment[]        = {10};
        static constexpr char UnlitSkinned2DVertex[]   = {11};
        static constexpr char UnlitSkinned2DFragment[] = {12};

        static constexpr char DefaultPostFXFragment[] = {31};

        static constexpr std::uint32_t MaxId = 31;
    };

    struct PipelineIds {
        static constexpr std::uint32_t None = 0;

        static constexpr std::uint32_t ShadowPassMeshes        = 1;
        static constexpr std::uint32_t ShadowPassSkinnedMeshes = 2;

        static constexpr std::uint32_t Meshes        = 3;
        static constexpr std::uint32_t SkinnedMeshes = 4;

        static constexpr std::uint32_t Lit2DGeometry          = 5;
        static constexpr std::uint32_t Unlit2DGeometry        = 6;
        static constexpr std::uint32_t LitSkinned2DGeometry   = 7;
        static constexpr std::uint32_t UnlitSkinned2DGeometry = 8;

        static constexpr std::uint32_t PostFXBase = 100;
    };

    struct RenderPassIds {
        static constexpr std::uint32_t OffScreenSceneRender   = 0;
        static constexpr std::uint32_t SwapchainPrimaryRender = 1;
    };
};

} // namespace render
} // namespace bl

#endif
