#ifndef BLIB_RENDER_CONFIG_HPP
#define BLIB_RENDER_CONFIG_HPP

#include <BLIB/Logging.hpp>
#include <cstdint>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
/**
 * @brief Collection of constants and settings for the renderer
 *
 * @ingroup Renderer
 */
struct Config {
    static constexpr unsigned int MaxConcurrentFrames = 2;

    static constexpr unsigned int MaxSceneObservers = 5;

    static constexpr std::size_t MaxDescriptorSets = 4;

    static constexpr std::size_t MaxDescriptorBindings = 8;

    static constexpr std::uint32_t MaxRenderPasses = 4;

    static constexpr std::size_t MaxRenderPhases = 8;

    static constexpr std::uint32_t MaxTextureCount = 4096;

    static constexpr std::uint32_t MaxRenderTextures = 32;

    static constexpr std::uint32_t ErrorTextureId = MaxTextureCount - Config::MaxRenderTextures - 1;

    static constexpr std::uint32_t DefaultSceneObjectCapacity = 128;

    static constexpr glm::vec3 UpDirection  = {0.f, 1.f, 0.f};
    static constexpr glm::vec3 Rotate2DAxis = {0.f, 0.f, 1.f};

    /**
     * @brief Special ids for built-in shaders
     */
    struct ShaderIds {
        static constexpr char EmptyVertex[] = {0};

        static constexpr char MeshVertex[]          = {1};
        static constexpr char MeshFragment[]        = {2};
        static constexpr char SkinnedMeshVertex[]   = {3};
        static constexpr char SkinnedMeshFragment[] = {4};

        static constexpr char Vertex2D[]        = {5};
        static constexpr char Fragment2DLit[]   = {6};
        static constexpr char Fragment2DUnlit[] = {7};

        static constexpr char Vertex2DSkinned[]           = {8};
        static constexpr char Fragment2DSkinnedUnlit[]    = {9};
        static constexpr char Fragment2DSkinnedLit[]      = {10};
        static constexpr char Fragment2DRotatedParticle[] = {11};

        static constexpr char TextFragment[]  = {12};
        static constexpr char SlideshowVert[] = {13};

        static constexpr char FadeEffectFragment[] = {31};

        static constexpr std::uint32_t MaxId = 31;
    };

    /**
     * @brief Built-in pipelines
     */
    struct PipelineIds {
        static constexpr std::uint32_t None = 0;

        static constexpr std::uint32_t LitMesh3D          = 1;
        static constexpr std::uint32_t UnlitMesh3D        = 2;
        static constexpr std::uint32_t LitSkinnedMesh3D   = 3;
        static constexpr std::uint32_t UnlitSkinnedMesh3D = 4;

        static constexpr std::uint32_t Lit2DGeometry               = 5;
        static constexpr std::uint32_t Unlit2DGeometry             = 6;
        static constexpr std::uint32_t Unlit2DGeometryNoDepthWrite = 7;
        static constexpr std::uint32_t LitSkinned2DGeometry        = 8;
        static constexpr std::uint32_t UnlitSkinned2DGeometry      = 9;

        static constexpr std::uint32_t Text           = 10;
        static constexpr std::uint32_t SlideshowLit   = 11;
        static constexpr std::uint32_t SlideshowUnlit = 12;
        static constexpr std::uint32_t Lines2D        = 13;

        static constexpr std::uint32_t FadeEffect = 100;
    };

    /**
     * @brief Built-in MaterialPipeline ids
     */
    struct MaterialPipelineIds {
        static constexpr std::uint32_t Mesh3D            = 1;
        static constexpr std::uint32_t Mesh3DSkinned     = 2;
        static constexpr std::uint32_t Geometry2D        = 3;
        static constexpr std::uint32_t Geometry2DSkinned = 4;
        static constexpr std::uint32_t Text              = 5;
        static constexpr std::uint32_t Slideshow2D       = 6;
        static constexpr std::uint32_t Lines2D           = 7;
    };

    /**
     * @brief Built-in render passes
     */
    struct RenderPassIds {
        static constexpr std::uint32_t StandardAttachmentDefault = 0;
        static constexpr std::uint32_t SwapchainDefault          = 1;
    };

    static constexpr std::uint32_t SPIRVMagicNumber = 0x07230203;
};

} // namespace rc
} // namespace bl

#endif
