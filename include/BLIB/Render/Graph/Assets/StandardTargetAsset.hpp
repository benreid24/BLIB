#ifndef BLIB_RENDER_GRAPH_ASSETS_STANDARDATTACHMENTASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_STANDARDATTACHMENTASSET_HPP

#include <BLIB/Render/Graph/Assets/GenericTargetAsset.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Shader resource for a standard single color attachment
 *
 * @ingroup Renderer
 */
using StandardTargetShaderResource = sri::AttachmentImageSetResource<
    1, std::array<vk::SemanticTextureFormat, 1>{vk::SemanticTextureFormat::Color},
    std::array<VkImageUsageFlags, 1>{VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                     VK_IMAGE_USAGE_SAMPLED_BIT},
    sri::TargetSize(sri::TargetSize::ObserverSize),
    sri::MSAABehavior::UseSettings | sri::MSAABehavior::ResolveAttachments>;

/**
 * @brief Helper typedef for standard target assets
 *
 * @ingroup Renderer
 */
using StandardTargetAsset =
    GenericTargetAsset<StandardTargetShaderResource, sr::StoreKey::Observer,
                       cfg::RenderPassIds::StandardAttachmentPass,
                       RenderPassBehavior::StartedByAsset, DepthAttachmentType::SharedDepthBuffer>;

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
