#ifndef BLIB_RENDER_CONFIG_HPP
#define BLIB_RENDER_CONFIG_HPP

#include <BLIB/Logging.hpp>
#include <array>
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

    static constexpr unsigned int MaxSceneObservers = 64;

    static constexpr std::size_t MaxDescriptorSets = 4;

    static constexpr std::size_t MaxDescriptorBindings = 8;

    static constexpr std::uint32_t MaxRenderPasses = 8;

    static constexpr std::size_t MaxRenderPhases = 8;

    static constexpr std::uint32_t MaxTextureCount = 4096;

    static constexpr std::uint32_t MaxRenderTextures = 32;

    static constexpr std::uint32_t MaxCubemapCount = 16;

    static constexpr std::uint32_t ErrorTextureId = MaxTextureCount - Config::MaxRenderTextures - 1;

    static constexpr std::uint32_t DefaultSceneObjectCapacity = 128;

    static constexpr std::uint32_t MaxPointLights  = 128;
    static constexpr std::uint32_t MaxSpotLights   = 128;
    static constexpr std::uint32_t MaxPointShadows = 16;
    static constexpr std::uint32_t MaxSpotShadows  = 16;

    static constexpr glm::vec3 UpDirection  = {0.f, 1.f, 0.f};
    static constexpr glm::vec3 Rotate2DAxis = {0.f, 0.f, 1.f};

    /**
     * @brief Special ids for built-in shaders
     */
    struct ShaderIds {
        static constexpr char EmptyVertex[] = {1, 0};

        static constexpr char MeshVertex[]        = {2, 0};
        static constexpr char MeshFragmentLit[]   = {3, 0};
        static constexpr char MeshFragmentUnlit[] = {4, 0};

        static constexpr char MeshVertexMaterial[]        = {5, 0};
        static constexpr char MeshFragmentMaterialLit[]   = {6, 0};
        static constexpr char MeshFragmentMaterialUnlit[] = {7, 0};

        static constexpr char MeshVertexSkinned[]        = {8, 0};
        static constexpr char MeshFragmentSkinnedLit[]   = {6, 0}; // MeshFragmentMaterialLit
        static constexpr char MeshFragmentSkinnedUnlit[] = {7, 0}; // MeshFragmentMaterialUnlit

        static constexpr char SkyboxVertex[]   = {9, 0};
        static constexpr char SkyboxFragment[] = {10, 0};

        static constexpr char Vertex2D[]        = {11, 0};
        static constexpr char Fragment2DLit[]   = {12, 0};
        static constexpr char Fragment2DUnlit[] = {13, 0};

        static constexpr char Vertex2DSkinned[]           = {14, 0};
        static constexpr char Fragment2DSkinnedUnlit[]    = {15, 0};
        static constexpr char Fragment2DSkinnedLit[]      = {16, 0};
        static constexpr char Fragment2DRotatedParticle[] = {17, 0};

        static constexpr char TextFragment[]  = {18, 0};
        static constexpr char SlideshowVert[] = {19, 0};

        static constexpr char ShadowVertex[]        = {20, 0};
        static constexpr char PointShadowVertex[]   = {21, 0};
        static constexpr char PointShadowGeometry[] = {22, 0};

        static constexpr char BloomBlurFragment[]            = {124, 0};
        static constexpr char BloomHighlightFilterFragment[] = {125, 0};
        static constexpr char PostProcess3DFragment[]        = {126, 0};
        static constexpr char FadeEffectFragment[]           = {127, 0};

        static constexpr std::uint32_t MaxId = 128;
    };

    /**
     * @brief Built-in pipelines
     */
    struct PipelineIds {
        static constexpr std::uint32_t None = 0;

        static constexpr std::uint32_t LitMesh3D           = 1;
        static constexpr std::uint32_t UnlitMesh3D         = 2;
        static constexpr std::uint32_t LitMesh3DMaterial   = 3;
        static constexpr std::uint32_t UnlitMesh3DMaterial = 4;
        static constexpr std::uint32_t LitMesh3DSkinned    = 5;
        static constexpr std::uint32_t UnlitMesh3DSkinned  = 6;
        static constexpr std::uint32_t Skybox              = 7;

        static constexpr std::uint32_t PostProcess3D        = 50;
        static constexpr std::uint32_t BloomHighlightFilter = 51;
        static constexpr std::uint32_t BloomBlur            = 52;

        static constexpr std::uint32_t Lit2DGeometry               = 100;
        static constexpr std::uint32_t Unlit2DGeometry             = 101;
        static constexpr std::uint32_t Unlit2DGeometryNoDepthWrite = 102;
        static constexpr std::uint32_t LitSkinned2DGeometry        = 103;
        static constexpr std::uint32_t UnlitSkinned2DGeometry      = 104;

        static constexpr std::uint32_t ShadowMapRegular      = 150;
        static constexpr std::uint32_t ShadowMapSkinned      = 151;
        static constexpr std::uint32_t PointShadowMapRegular = 152;
        static constexpr std::uint32_t PointShadowMapSkinned = 153;

        static constexpr std::uint32_t Text           = 200;
        static constexpr std::uint32_t SlideshowLit   = 201;
        static constexpr std::uint32_t SlideshowUnlit = 202;
        static constexpr std::uint32_t Lines2D        = 203;

        static constexpr std::uint32_t FadeEffect = 300;
    };

    /**
     * @brief Built-in MaterialPipeline ids
     */
    struct MaterialPipelineIds {
        static constexpr std::uint32_t Mesh3D            = 1;
        static constexpr std::uint32_t Mesh3DMaterial    = 2;
        static constexpr std::uint32_t Mesh3DSkinned     = 3;
        static constexpr std::uint32_t Geometry2D        = 4;
        static constexpr std::uint32_t Geometry2DSkinned = 5;
        static constexpr std::uint32_t Text              = 6;
        static constexpr std::uint32_t Slideshow2D       = 7;
        static constexpr std::uint32_t Lines2D           = 8;
        static constexpr std::uint32_t Skybox            = 9;
    };

    /**
     * @brief Built-in render passes
     */
    struct RenderPassIds {
        static constexpr std::uint32_t StandardAttachmentDefault = 0;
        static constexpr std::uint32_t SwapchainDefault          = 1;
        static constexpr std::uint32_t HDRAttachmentDefault      = 2;
        static constexpr std::uint32_t BloomPass                 = 3;
        static constexpr std::uint32_t ShadowMapPass             = 4;
    };

    static constexpr std::uint32_t SPIRVMagicNumber = 0x07230203;

    static constexpr std::array<glm::vec3, 6> CubemapDirections = {
        glm::vec3(1.f, 0.f, 0.f),  // Right
        glm::vec3(-1.f, 0.f, 0.f), // Left
        glm::vec3(0.f, 1.f, 0.f),  // Up
        glm::vec3(0.f, -1.f, 0.f), // Down
        glm::vec3(0.f, 0.f, 1.f),  // Back
        glm::vec3(0.f, 0.f, -1.f)  // Front
    };
};

} // namespace rc
} // namespace bl

#endif
