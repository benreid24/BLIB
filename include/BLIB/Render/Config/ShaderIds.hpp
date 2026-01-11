#ifndef BLIB_RENDER_CONFIG_SHADERIDS_HPP
#define BLIB_RENDER_CONFIG_SHADERIDS_HPP

#include <cstdint>

namespace bl
{
namespace rc
{
namespace cfg
{
/**
 * @brief Special ids for built-in shaders
 *
 * @ingroup Renderer
 */
struct ShaderIds {
    static constexpr char EmptyVertex[]      = {1, 0};
    static constexpr char FullscreenVertex[] = {2, 0};

    static constexpr char MeshVertex[]           = {3, 0};
    static constexpr char MeshFragment[]         = {4, 0};
    static constexpr char MeshFragmentDeferred[] = {5, 0};

    static constexpr char MeshVertexMaterial[]           = {6, 0};
    static constexpr char MeshFragmentMaterial[]         = {7, 0};
    static constexpr char MeshFragmentMaterialDeferred[] = {8, 0};

    static constexpr char MeshVertexSkinned[]           = {9, 0};
    static constexpr char MeshFragmentSkinned[]         = {MeshFragmentMaterial[0], 0};
    static constexpr char MeshFragmentSkinnedDeferred[] = {MeshFragmentMaterialDeferred[0], 0};

    static constexpr char SkyboxVertex[]   = {10, 0};
    static constexpr char SkyboxFragment[] = {11, 0};

    static constexpr char Outline3DVertex[]        = {12, 0};
    static constexpr char Outline3DVertexSkinned[] = {13, 0};
    static constexpr char Outline3DFragment[]      = {14, 0};

    static constexpr char DeferredLightVolumeVertex[]           = {15, 0};
    static constexpr char DeferredLightVolumeFragment[]         = {16, 0};
    static constexpr char DeferredLightVolumeResolvedFragment[] = {17, 0};
    static constexpr char DeferredLightVolumeSampledFragment[]  = {18, 0};

    static constexpr char SSAOBlurFragment[]       = {19, 0};
    static constexpr char SSAOGenFragment[]        = {20, 0};
    static constexpr char SSAOGenResolveFragment[] = {21, 0};
    static constexpr char SSAOGenSampledFragment[] = {22, 0};

    static constexpr char Vertex2D[]        = {23, 0};
    static constexpr char Fragment2DLit[]   = {24, 0};
    static constexpr char Fragment2DUnlit[] = {25, 0};

    static constexpr char Vertex2DSkinned[]           = {26, 0};
    static constexpr char Fragment2DSkinnedUnlit[]    = {27, 0};
    static constexpr char Fragment2DSkinnedLit[]      = {28, 0};
    static constexpr char Fragment2DRotatedParticle[] = {29, 0};

    static constexpr char TextFragment[]  = {30, 0};
    static constexpr char SlideshowVert[] = {31, 0};

    static constexpr char ShadowVertex[]             = {32, 0};
    static constexpr char ShadowVertexSkinned[]      = {33, 0};
    static constexpr char PointShadowVertex[]        = {34, 0};
    static constexpr char PointShadowVertexSkinned[] = {35, 0};
    static constexpr char PointShadowGeometry[]      = {36, 0};
    static constexpr char PointShadowFragment[]      = {37, 0};

    static constexpr char AutoExposureAccumulate[] = {100, 0};
    static constexpr char AutoExposureAdjust[]     = {101, 0};

    static constexpr char BloomBlurFragment[]            = {124, 0};
    static constexpr char BloomHighlightFilterFragment[] = {125, 0};
    static constexpr char PostProcess3DFragment[]        = {126, 0};
    static constexpr char FadeEffectFragment[]           = {127, 0};

    static constexpr std::uint32_t MaxId = 128;
};

} // namespace cfg
} // namespace rc
} // namespace bl

#endif
