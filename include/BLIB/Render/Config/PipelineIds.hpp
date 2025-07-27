#ifndef BLIB_RENDER_CONFIG_PIPELINEIDS_HPP
#define BLIB_RENDER_CONFIG_PIPELINEIDS_HPP

#include <cstdint>

namespace bl
{
namespace rc
{
namespace cfg
{
/**
 * @brief Built-in pipelines
 *
 * @ingroup Renderer
 */
struct PipelineIds {
    static constexpr std::uint32_t None = 0;

    static constexpr std::uint32_t Mesh3D                 = 1;
    static constexpr std::uint32_t Mesh3DMaterial         = 2;
    static constexpr std::uint32_t Mesh3DSkinned          = 3;
    static constexpr std::uint32_t DeferredMesh3D         = 4;
    static constexpr std::uint32_t DeferredMesh3DMaterial = 5;
    static constexpr std::uint32_t DeferredMesh3DSkinned  = 6;
    static constexpr std::uint32_t Skybox                 = 7;
    static constexpr std::uint32_t Outline3D              = 8;
    static constexpr std::uint32_t Outline3DSkinned       = 9;

    static constexpr std::uint32_t PostProcess3D        = 50;
    static constexpr std::uint32_t BloomHighlightFilter = 51;
    static constexpr std::uint32_t BloomBlur            = 52;
    static constexpr std::uint32_t DeferredLightVolume  = 53;
    static constexpr std::uint32_t SSAOGen              = 54;
    static constexpr std::uint32_t SSAOBlur             = 55;

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

} // namespace cfg
} // namespace rc
} // namespace bl

#endif
