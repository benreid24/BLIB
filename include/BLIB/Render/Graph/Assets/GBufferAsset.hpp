#ifndef BLIB_RENDER_GRAPH_ASSETS_GBUFFERASSETS_HPP
#define BLIB_RENDER_GRAPH_ASSETS_GBUFFERASSETS_HPP

#include <BLIB/Render/Config/RenderPassIds.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/GenericTargetAsset.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Shader resource for the GBuffer used in deferred rendering
 *
 * @ingroup Renderer
 */
using GBufferShaderResource = sri::AttachmentImageSetResource<
    4,
    std::array<vk::SemanticTextureFormat, 4>{vk::SemanticTextureFormat::Color,
                                             vk::SemanticTextureFormat::Color,
                                             vk::SemanticTextureFormat::HighPrecisionColor,
                                             vk::SemanticTextureFormat::HighPrecisionColor},
    std::array<VkImageUsageFlags, 4>{
        // albedo
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        // specular + shininess
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        // positions + lighting on/off
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        // normals
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT},
    sri::TargetSize(sri::TargetSize::ObserverSize), sri::MSAABehavior::UseSettings>;

/**
 * @brief Shader resource key for the GBuffer used in deferred rendering
 *
 * @ingroup Renderer
 */
constexpr sr::Key<GBufferShaderResource> GBufferShaderResourceKey =
    makeShaderResourceKey<GBufferShaderResource>(rg::AssetTags::GBuffer);

/**
 * @brief GBuffer asset for deferred rendering
 *
 * @ingroup Renderer
 */
using GBufferAsset =
    GenericTargetAsset<GBufferShaderResource, sr::StoreKey::Observer,
                       cfg::RenderPassIds::DeferredObjectPass, RenderPassBehavior::StartedByAsset,
                       DepthAttachmentType::SharedDepthBuffer>;

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
