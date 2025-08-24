#ifndef BLIB_RENDER_EVENTS_SHADOWMAPSINVALIDATED_HPP
#define BLIB_RENDER_EVENTS_SHADOWMAPSINVALIDATED_HPP

namespace bl
{
namespace rc
{
namespace rgi
{
class ShadowMapAsset;
}
namespace event
{
/**
 * @brief Fired when shadow maps are re-created and descriptors need to be updated
 *
 * @ingroup Renderer
 */
struct ShadowMapsInvalidated {
    rgi::ShadowMapAsset* asset;
};

} // namespace event
} // namespace rc
} // namespace bl

#endif
