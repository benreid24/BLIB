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
    static constexpr char EmptyVertex[] = {1, 0};

    static constexpr char MeshVertex[]           = {2, 0};
    static constexpr char MeshFragment[]         = {3, 0};
    static constexpr char MeshFragmentDeferred[] = {4, 0};

    static constexpr char MeshVertexMaterial[]           = {5, 0};
    static constexpr char MeshFragmentMaterial[]         = {6, 0};
    static constexpr char MeshFragmentMaterialDeferred[] = {7, 0};

    static constexpr char MeshVertexSkinned[]           = {8, 0};
    static constexpr char MeshFragmentSkinned[]         = {6, 0}; // MeshFragmentMaterial
    static constexpr char MeshFragmentSkinnedDeferred[] = {7, 0}; // MeshFragmentMaterialDeferred

    static constexpr char SkyboxVertex[]   = {9, 0};
    static constexpr char SkyboxFragment[] = {10, 0};

    static constexpr char Outline3DVertex[]        = {11, 0};
    static constexpr char Outline3DVertexSkinned[] = {12, 0};
    static constexpr char Outline3DFragment[]      = {13, 0};

    static constexpr char Vertex2D[]        = {14, 0};
    static constexpr char Fragment2DLit[]   = {15, 0};
    static constexpr char Fragment2DUnlit[] = {16, 0};

    static constexpr char Vertex2DSkinned[]           = {17, 0};
    static constexpr char Fragment2DSkinnedUnlit[]    = {18, 0};
    static constexpr char Fragment2DSkinnedLit[]      = {19, 0};
    static constexpr char Fragment2DRotatedParticle[] = {20, 0};

    static constexpr char TextFragment[]  = {21, 0};
    static constexpr char SlideshowVert[] = {22, 0};

    static constexpr char ShadowVertex[]        = {23, 0};
    static constexpr char PointShadowVertex[]   = {24, 0};
    static constexpr char PointShadowGeometry[] = {25, 0};
    static constexpr char PointShadowFragment[] = {26, 0};

    static constexpr char DeferredCompositeFragment[]    = {123, 0};
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
