#ifndef BLIB_RENDER_GRAPH_ASSETS_BLOOMASSETS_HPP
#define BLIB_RENDER_GRAPH_ASSETS_BLOOMASSETS_HPP

#include <BLIB/Render/Config/RenderPassIds.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/GenericTargetAsset.hpp>
#include <BLIB/Render/Graph/MultiAsset.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Shader resource for the bloom pass color attachment
 *
 * @ingroup Renderer
 */
using BloomColorAttachmentResource = sri::AttachmentImageSetResource<
    1, std::array<vk::SemanticTextureFormat, 1>{vk::SemanticTextureFormat::SFloatR16G16B16A16},
    std::array<VkImageUsageFlags, 1>{VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                     VK_IMAGE_USAGE_SAMPLED_BIT},
    sri::TargetSize(sri::TargetSize::ObserverSize), sri::MSAABehavior::Disabled>;

/**
 * @brief Single color attachment asset for the bloom pass
 *
 * @ingroup Renderer
 */
using BloomColorAttachmentAsset =
    rgi::GenericTargetAsset<BloomColorAttachmentResource, sr::StoreKey::Observer,
                            cfg::RenderPassIds::BloomPass, RenderPassBehavior::StartedByTask,
                            DepthAttachmentType::None>;

/**
 * @brief Pair of BloomColorAttachment used for the bloom pass
 *
 * @ingroup Renderer
 */
using BloomColorAttachmentPairAsset = rg::MultiAsset<BloomColorAttachmentAsset, 2>;

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
