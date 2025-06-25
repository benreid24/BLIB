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

    static constexpr char MeshVertex[]   = {2, 0};
    static constexpr char MeshFragment[] = {3, 0};

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
    static constexpr char PointShadowFragment[] = {23, 0};

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
