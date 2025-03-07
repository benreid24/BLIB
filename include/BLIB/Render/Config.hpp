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
        static constexpr char EmptyVertex[] = {0, 0};

        static constexpr char MeshVertex[]        = {1, 0};
        static constexpr char MeshFragmentLit[]   = {2, 0};
        static constexpr char MeshFragmentUnlit[] = {3, 0};

        static constexpr char MeshVertexMaterial[]        = {4, 0};
        static constexpr char MeshFragmentMaterialLit[]   = {5, 0};
        static constexpr char MeshFragmentMaterialUnlit[] = {6, 0};

        static constexpr char MeshVertexSkinned[]        = {7, 0};
        static constexpr char MeshFragmentSkinnedLit[]   = {5, 0}; // MeshFragmentMaterialLit
        static constexpr char MeshFragmentSkinnedUnlit[] = {6, 0}; // MeshFragmentMaterialUnlit

        static constexpr char Vertex2D[]        = {8, 0};
        static constexpr char Fragment2DLit[]   = {9, 0};
        static constexpr char Fragment2DUnlit[] = {10, 0};

        static constexpr char Vertex2DSkinned[]           = {11, 0};
        static constexpr char Fragment2DSkinnedUnlit[]    = {12, 0};
        static constexpr char Fragment2DSkinnedLit[]      = {13, 0};
        static constexpr char Fragment2DRotatedParticle[] = {14, 0};

        static constexpr char TextFragment[]  = {15, 0};
        static constexpr char SlideshowVert[] = {16, 0};

        static constexpr char FadeEffectFragment[] = {127, 0};

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

        static constexpr std::uint32_t Lit2DGeometry               = 100;
        static constexpr std::uint32_t Unlit2DGeometry             = 101;
        static constexpr std::uint32_t Unlit2DGeometryNoDepthWrite = 102;
        static constexpr std::uint32_t LitSkinned2DGeometry        = 103;
        static constexpr std::uint32_t UnlitSkinned2DGeometry      = 104;

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
