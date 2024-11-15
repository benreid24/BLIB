#include <BLIB/Render/Resources/PipelineCache.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/Builtin/FadeEffectFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Object2DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Object3DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene2DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene3DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/SlideshowFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/TexturePoolFactory.hpp>
#include <BLIB/Render/Primitives/SlideshowVertex.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
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

bool PipelineCache::pipelineExists(std::uint32_t pid) const {
    return cache.find(pid) != cache.end();
}

void PipelineCache::createBuiltins() {
    VkPipelineDepthStencilStateCreateInfo depthStencilDepthEnabled{};
    depthStencilDepthEnabled.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilDepthEnabled.depthTestEnable       = VK_TRUE;
    depthStencilDepthEnabled.depthWriteEnable      = VK_TRUE;
    depthStencilDepthEnabled.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilDepthEnabled.depthBoundsTestEnable = VK_FALSE;
    depthStencilDepthEnabled.minDepthBounds        = 0.0f; // Optional
    depthStencilDepthEnabled.maxDepthBounds        = 1.0f; // Optional
    depthStencilDepthEnabled.stencilTestEnable     = VK_FALSE;
    depthStencilDepthEnabled.front                 = {}; // Optional (Stencil)
    depthStencilDepthEnabled.back                  = {}; // Optional (Stencil)

    VkPipelineDepthStencilStateCreateInfo depthStencilDepthDisabled = depthStencilDepthEnabled;
    depthStencilDepthDisabled.depthTestEnable                       = VK_FALSE;
    depthStencilDepthDisabled.depthWriteEnable                      = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depthStencilDepthWriteDisabled = depthStencilDepthEnabled;
    depthStencilDepthWriteDisabled.depthTestEnable                       = VK_TRUE;
    depthStencilDepthWriteDisabled.depthWriteEnable                      = VK_FALSE;

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

    createPipline(Config::PipelineIds::SkinnedMeshes,
                  vk::PipelineParameters({Config::RenderPassIds::StandardAttachmentDefault,
                                          Config::RenderPassIds::SwapchainDefault})
                      .withShaders(Config::ShaderIds::SkinnedMeshVertex,
                                   Config::ShaderIds::SkinnedMeshFragment)
                      .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                      .withRasterizer(rasterizer)
                      .withDepthStencilState(&depthStencilDepthEnabled)
                      .addDescriptorSet<ds::TexturePoolFactory>()
                      .addDescriptorSet<ds::Scene3DFactory>()
                      .addDescriptorSet<ds::Object3DFactory>()
                      .build());

    createPipline(Config::PipelineIds::LitSkinned2DGeometry,
                  vk::PipelineParameters({Config::RenderPassIds::StandardAttachmentDefault,
                                          Config::RenderPassIds::SwapchainDefault})
                      .withShaders(Config::ShaderIds::Vertex2DSkinned,
                                   Config::ShaderIds::Fragment2DSkinnedLit)
                      .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                      .withRasterizer(rasterizer)
                      .withDepthStencilState(&depthStencilDepthEnabled)
                      .addDescriptorSet<ds::TexturePoolFactory>()
                      .addDescriptorSet<ds::Scene2DFactory>()
                      .addDescriptorSet<ds::Object2DFactory>()
                      .build());

    createPipline(Config::PipelineIds::UnlitSkinned2DGeometry,
                  vk::PipelineParameters({Config::RenderPassIds::StandardAttachmentDefault,
                                          Config::RenderPassIds::SwapchainDefault})
                      .withShaders(Config::ShaderIds::Vertex2DSkinned,
                                   Config::ShaderIds::Fragment2DSkinnedUnlit)
                      .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                      .withRasterizer(rasterizer)
                      .withDepthStencilState(&depthStencilDepthEnabled)
                      .addDescriptorSet<ds::TexturePoolFactory>()
                      .addDescriptorSet<ds::Scene2DFactory>()
                      .addDescriptorSet<ds::Object2DFactory>()
                      .build());

    createPipline(Config::PipelineIds::Lit2DGeometry,
                  vk::PipelineParameters({Config::RenderPassIds::StandardAttachmentDefault,
                                          Config::RenderPassIds::SwapchainDefault})
                      .withShaders(Config::ShaderIds::Vertex2D, Config::ShaderIds::Fragment2DLit)
                      .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                      .withRasterizer(rasterizer)
                      .withDepthStencilState(&depthStencilDepthEnabled)
                      .addDescriptorSet<ds::Scene2DFactory>()
                      .addDescriptorSet<ds::Object2DFactory>()
                      .build());

    createPipline(Config::PipelineIds::Unlit2DGeometry,
                  vk::PipelineParameters({Config::RenderPassIds::StandardAttachmentDefault,
                                          Config::RenderPassIds::SwapchainDefault})
                      .withShaders(Config::ShaderIds::Vertex2D, Config::ShaderIds::Fragment2DUnlit)
                      .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                      .withRasterizer(rasterizer)
                      .withDepthStencilState(&depthStencilDepthEnabled)
                      .addDescriptorSet<ds::Scene2DFactory>()
                      .addDescriptorSet<ds::Object2DFactory>()
                      .build());

    createPipline(Config::PipelineIds::Lines2D,
                  vk::PipelineParameters({Config::RenderPassIds::StandardAttachmentDefault,
                                          Config::RenderPassIds::SwapchainDefault})
                      .withShaders(Config::ShaderIds::Vertex2D, Config::ShaderIds::Fragment2DUnlit)
                      .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_LINE_LIST)
                      .withRasterizer(rasterizer)
                      .withDepthStencilState(&depthStencilDepthEnabled)
                      .addDescriptorSet<ds::Scene2DFactory>()
                      .addDescriptorSet<ds::Object2DFactory>()
                      .build());

    createPipline(Config::PipelineIds::Unlit2DGeometryNoDepthWrite,
                  vk::PipelineParameters({Config::RenderPassIds::StandardAttachmentDefault,
                                          Config::RenderPassIds::SwapchainDefault})
                      .withShaders(Config::ShaderIds::Vertex2D, Config::ShaderIds::Fragment2DUnlit)
                      .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                      .withRasterizer(rasterizer)
                      .withDepthStencilState(&depthStencilDepthWriteDisabled)
                      .addDescriptorSet<ds::Scene2DFactory>()
                      .addDescriptorSet<ds::Object2DFactory>()
                      .build());

    createPipline(
        Config::PipelineIds::Text,
        vk::PipelineParameters({Config::RenderPassIds::StandardAttachmentDefault,
                                Config::RenderPassIds::SwapchainDefault})
            .withShaders(Config::ShaderIds::Vertex2DSkinned, Config::ShaderIds::TextFragment)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withRasterizer(rasterizer)
            .withDepthStencilState(&depthStencilDepthEnabled)
            .addDescriptorSet<ds::TexturePoolFactory>()
            .addDescriptorSet<ds::Scene2DFactory>()
            .addDescriptorSet<ds::Object2DFactory>()
            .build());

    createPipline(
        Config::PipelineIds::SlideshowLit,
        vk::PipelineParameters({Config::RenderPassIds::StandardAttachmentDefault,
                                Config::RenderPassIds::SwapchainDefault})
            .withShaders(Config::ShaderIds::SlideshowVert, Config::ShaderIds::Fragment2DSkinnedLit)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withRasterizer(rasterizer)
            .withVertexFormat(prim::SlideshowVertex::bindingDescription(),
                              prim::SlideshowVertex::attributeDescriptions())
            .withDepthStencilState(&depthStencilDepthEnabled)
            .addDescriptorSet<ds::TexturePoolFactory>()
            .addDescriptorSet<ds::Scene2DFactory>()
            .addDescriptorSet<ds::Object2DFactory>()
            .addDescriptorSet<ds::SlideshowFactory>()
            .build());

    createPipline(Config::PipelineIds::SlideshowUnlit,
                  vk::PipelineParameters({Config::RenderPassIds::StandardAttachmentDefault,
                                          Config::RenderPassIds::SwapchainDefault})
                      .withShaders(Config::ShaderIds::SlideshowVert,
                                   Config::ShaderIds::Fragment2DSkinnedUnlit)
                      .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                      .withRasterizer(rasterizer)
                      .withVertexFormat(prim::SlideshowVertex::bindingDescription(),
                                        prim::SlideshowVertex::attributeDescriptions())
                      .withDepthStencilState(&depthStencilDepthEnabled)
                      .addDescriptorSet<ds::TexturePoolFactory>()
                      .addDescriptorSet<ds::Scene2DFactory>()
                      .addDescriptorSet<ds::Object2DFactory>()
                      .addDescriptorSet<ds::SlideshowFactory>()
                      .build());

    createPipline(
        Config::PipelineIds::FadeEffect,
        vk::PipelineParameters({Config::RenderPassIds::StandardAttachmentDefault,
                                Config::RenderPassIds::SwapchainDefault})
            .withShaders(Config::ShaderIds::EmptyVertex, Config::ShaderIds::FadeEffectFragment)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withRasterizer(rasterizer)
            .withDepthStencilState(&depthStencilDepthDisabled)
            .addDescriptorSet<ds::FadeEffectFactory>()
            .addPushConstantRange(0, sizeof(float), VK_SHADER_STAGE_FRAGMENT_BIT)
            .build());
}

} // namespace res
} // namespace rc
} // namespace bl
