#include <BLIB/Render/Resources/PipelineCache.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/Builtin/Object2DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Object3DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/PostFXFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene2DLitFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene3DUnlitFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/TexturePoolFactory.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
namespace res
{
PipelineCache::PipelineCache(Renderer& r)
: renderer(r) {
    // TODO - leverage VkPipelineCache?
}

void PipelineCache::cleanup() { cache.clear(); }

vk::Pipeline& PipelineCache::createPipline(std::uint32_t id, vk::PipelineParameters&& params) {
    const auto insertResult =
        cache.try_emplace(id, renderer, std::forward<vk::PipelineParameters>(params));
    if (!insertResult.second) { BL_LOG_WARN << "Pipeline with id " << id << " already exists"; }
    return insertResult.first->second;
}

vk::Pipeline& PipelineCache::getPipeline(std::uint32_t id) {
    auto it = cache.find(id);
    if (it == cache.end()) {
        BL_LOG_CRITICAL << "Failed to find pipeline with id: " << id;
        throw std::runtime_error("Failed to find pipeline");
    }
    return it->second;
}

void PipelineCache::createBuiltins() {
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable       = VK_TRUE;
    depthStencil.depthWriteEnable      = VK_TRUE;
    depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds        = 0.0f; // Optional
    depthStencil.maxDepthBounds        = 1.0f; // Optional
    depthStencil.stencilTestEnable     = VK_FALSE;
    depthStencil.front                 = {}; // Optional (Stencil)
    depthStencil.back                  = {}; // Optional (Stencil)

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = VK_CULL_MODE_NONE;
    rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp          = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor    = 0.0f; // Optional

    createPipline(Config::PipelineIds::Meshes,
                  vk::PipelineParameters({Config::RenderPassIds::OffScreenSceneRender,
                                          Config::RenderPassIds::SwapchainPrimaryRender})
                      .withShaders(Config::ShaderIds::MeshVertex, Config::ShaderIds::MeshFragment)
                      .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                      .withRasterizer(rasterizer)
                      .withDepthStencilState(&depthStencil)
                      .addDescriptorSet<ds::Scene3DUnlitFactory>()
                      .addDescriptorSet<ds::Object3DFactory>()
                      .build());

    createPipline(Config::PipelineIds::SkinnedMeshes,
                  vk::PipelineParameters({Config::RenderPassIds::OffScreenSceneRender,
                                          Config::RenderPassIds::SwapchainPrimaryRender})
                      .withShaders(Config::ShaderIds::SkinnedMeshVertex,
                                   Config::ShaderIds::SkinnedMeshFragment)
                      .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                      .withRasterizer(rasterizer)
                      .withDepthStencilState(&depthStencil)
                      .addDescriptorSet<ds::TexturePoolFactory>()
                      .addDescriptorSet<ds::Scene3DUnlitFactory>()
                      .addDescriptorSet<ds::Object3DFactory>()
                      .build());

    createPipline(Config::PipelineIds::LitSkinned2DGeometry,
                  vk::PipelineParameters({Config::RenderPassIds::OffScreenSceneRender,
                                          Config::RenderPassIds::SwapchainPrimaryRender})
                      .withShaders(Config::ShaderIds::LitSkinned2DVertex,
                                   Config::ShaderIds::LitSkinned2DFragment)
                      .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                      .withRasterizer(rasterizer)
                      .withDepthStencilState(&depthStencil)
                      .addDescriptorSet<ds::TexturePoolFactory>()
                      .addDescriptorSet<ds::Scene2DLitFactory>()
                      .addDescriptorSet<ds::Object2DFactory>()
                      .build());

    // TODO - other 2d pipelines

    createPipline(
        Config::PipelineIds::PostFXBase,
        vk::PipelineParameters({Config::RenderPassIds::SwapchainPrimaryRender})
            .withShaders(Config::ShaderIds::EmptyVertex, Config::ShaderIds::DefaultPostFXFragment)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withRasterizer(rasterizer)
            .withDepthStencilState(&depthStencil)
            .addDescriptorSet<ds::PostFXFactory>()
            .build());
}

} // namespace res
} // namespace render
} // namespace bl
