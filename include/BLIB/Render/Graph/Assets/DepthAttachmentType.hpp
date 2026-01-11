#ifndef BLIB_RENDER_GRAPH_ASSETS_DEPTHATTACHMENTTYPE_HPP
#define BLIB_RENDER_GRAPH_ASSETS_DEPTHATTACHMENTTYPE_HPP

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Represents the type of depth attachment to use for a render target asset
 *
 * @ingroup Renderer
 */
enum struct DepthAttachmentType {
    /// The asset has no depth attachment
    None,

    /// The asset uses the shared graph depth buffer
    SharedDepthBuffer
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
