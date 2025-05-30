#ifndef BLIB_RENDER_SCENES_EXTRACONTEXTS_HPP
#define BLIB_RENDER_SCENES_EXTRACONTEXTS_HPP

namespace bl
{
namespace rc
{
namespace scene
{
// Namespace containing context extensions for SceneRenderContext
namespace ctx
{
/**
 * @brief Extra context for when shadow maps are being rendered
 *
 * @ingroup Renderer
 */
struct ShadowMapContext {
    enum LightType { SunLight, SpotLight, PointLight };

    LightType lightType;
    unsigned int lightIndex;
};

} // namespace ctx
} // namespace scene
} // namespace rc
} // namespace bl

#endif
