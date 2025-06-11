#include <BLIB/Render/Resources/PipelineCache.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/Builtin/ColorAttachmentFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/GlobalDataFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Object2DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Object3DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene2DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene3DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/ShadowMapFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/SlideshowFactory.hpp>
#include <BLIB/Render/Primitives/SlideshowVertex.hpp>
#include <BLIB/Render/Primitives/Vertex3D.hpp>
#include <BLIB/Render/Primitives/Vertex3DSkinned.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace res
{
PipelineCache::PipelineCache(Renderer& r)
: renderer(r)
, nextId(DynamicPipelineIdStart) {
    // TODO - leverage VkPipelineCache?
}

void PipelineCache::cleanup() { cache.clear(); }

vk::Pipeline& PipelineCache::createPipeline(std::uint32_t id, vk::PipelineParameters&& params) {
    const auto insertResult =
        cache.try_emplace(id, renderer, id, std::forward<vk::PipelineParameters>(params));
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

vk::Pipeline& PipelineCache::getOrCreatePipeline(vk::PipelineParameters&& params) {
    for (auto& p : cache) {
        if (p.second.getCreationParameters() == params) { return p.second; }
    }

    std::uint32_t newId = nextId;
    while (pipelineExists(newId)) { ++newId; }
    nextId = newId + 1;
    return createPipeline(newId, std::forward<vk::PipelineParameters>(params));
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

    VkPipelineRasterizationStateCreateInfo rasterizer3d = rasterizer;
    rasterizer3d.cullMode                               = VK_CULL_MODE_BACK_BIT;
    rasterizer3d.frontFace                              = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    VkPipelineRasterizationStateCreateInfo skyboxRasterizer = rasterizer3d;
    skyboxRasterizer.cullMode                               = VK_CULL_MODE_FRONT_BIT;

    VkPipelineRasterizationStateCreateInfo rasterizerShadow = rasterizer3d;
    rasterizerShadow.cullMode                               = VK_CULL_MODE_FRONT_BIT;
    rasterizerShadow.depthBiasEnable                        = VK_TRUE;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    createPipeline(
        Config::PipelineIds::LitMesh3D,
        vk::PipelineParameters()
            .withShaders(Config::ShaderIds::MeshVertex, Config::ShaderIds::MeshFragmentLit)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withVertexFormat(prim::Vertex3D::bindingDescription(),
                              prim::Vertex3D::attributeDescriptions())
            .withRasterizer(rasterizer3d)
            .withDepthStencilState(&depthStencilDepthEnabled)
            .addDescriptorSet<ds::GlobalDataFactory>()
            .addDescriptorSet<ds::Scene3DFactory>()
            .addDescriptorSet<ds::Object3DFactory>()
            .build());

    createPipeline(
        Config::PipelineIds::UnlitMesh3D,
        vk::PipelineParameters()
            .withShaders(Config::ShaderIds::MeshVertex, Config::ShaderIds::MeshFragmentUnlit)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withVertexFormat(prim::Vertex3D::bindingDescription(),
                              prim::Vertex3D::attributeDescriptions())
            .withRasterizer(rasterizer3d)
            .withDepthStencilState(&depthStencilDepthEnabled)
            .addDescriptorSet<ds::GlobalDataFactory>()
            .addDescriptorSet<ds::Scene3DFactory>()
            .addDescriptorSet<ds::Object3DFactory>()
            .build());

    createPipeline(Config::PipelineIds::LitMesh3DMaterial,
                   vk::PipelineParameters()
                       .withShaders(Config::ShaderIds::MeshVertexMaterial,
                                    Config::ShaderIds::MeshFragmentMaterialLit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withVertexFormat(prim::Vertex3D::bindingDescription(),
                                         prim::Vertex3D::attributeDescriptions())
                       .withRasterizer(rasterizer3d)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene3DFactory>()
                       .addDescriptorSet<ds::Object3DFactory>()
                       .build());

    createPipeline(Config::PipelineIds::UnlitMesh3DMaterial,
                   vk::PipelineParameters()
                       .withShaders(Config::ShaderIds::MeshVertexMaterial,
                                    Config::ShaderIds::MeshFragmentMaterialUnlit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withVertexFormat(prim::Vertex3D::bindingDescription(),
                                         prim::Vertex3D::attributeDescriptions())
                       .withRasterizer(rasterizer3d)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene3DFactory>()
                       .addDescriptorSet<ds::Object3DFactory>()
                       .build());

    createPipeline(Config::PipelineIds::LitMesh3DSkinned,
                   vk::PipelineParameters()
                       .withShaders(Config::ShaderIds::MeshVertexSkinned,
                                    Config::ShaderIds::MeshFragmentSkinnedLit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withVertexFormat(prim::Vertex3D::bindingDescription(),
                                         prim::Vertex3D::attributeDescriptions())
                       .withRasterizer(rasterizer3d)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene3DFactory>()
                       .addDescriptorSet<ds::Object3DFactory>()
                       .build());

    createPipeline(Config::PipelineIds::UnlitMesh3DSkinned,
                   vk::PipelineParameters()
                       .withShaders(Config::ShaderIds::MeshVertexSkinned,
                                    Config::ShaderIds::MeshFragmentSkinnedUnlit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withVertexFormat(prim::Vertex3D::bindingDescription(),
                                         prim::Vertex3D::attributeDescriptions())
                       .withRasterizer(rasterizer3d)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene3DFactory>()
                       .addDescriptorSet<ds::Object3DFactory>()
                       .build());

    createPipeline(
        Config::PipelineIds::Skybox,
        vk::PipelineParameters()
            .withShaders(Config::ShaderIds::SkyboxVertex, Config::ShaderIds::SkyboxFragment)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withVertexFormat(prim::Vertex3D::bindingDescription(),
                              prim::Vertex3D::attributeDescriptions())
            .withRasterizer(skyboxRasterizer)
            .withDepthStencilState(&depthStencilDepthEnabled)
            .addDescriptorSet<ds::GlobalDataFactory>()
            .addDescriptorSet<ds::Scene3DFactory>()
            .addDescriptorSet<ds::Object3DFactory>()
            .build());

    createPipeline(Config::PipelineIds::ShadowMapRegular,
                   vk::PipelineParameters()
                       .withShader(Config::ShaderIds::ShadowVertex, VK_SHADER_STAGE_VERTEX_BIT)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withVertexFormat(prim::Vertex3D::bindingDescription(),
                                         prim::Vertex3D::attributeDescriptions())
                       .withRasterizer(rasterizerShadow)
                       .addDynamicStates({VK_DYNAMIC_STATE_DEPTH_BIAS})
                       .withSimpleColorBlendState(vk::PipelineParameters::ColorBlendBehavior::None)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::ShadowMapFactory>()
                       .addDescriptorSet<ds::Object3DFactory>()
                       .build());

    createPipeline(Config::PipelineIds::ShadowMapSkinned,
                   vk::PipelineParameters()
                       .withShader(Config::ShaderIds::ShadowVertex, VK_SHADER_STAGE_VERTEX_BIT)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withVertexFormat(prim::Vertex3DSkinned::bindingDescription(),
                                         prim::Vertex3DSkinned::attributeDescriptions())
                       .withRasterizer(rasterizerShadow)
                       .addDynamicStates({VK_DYNAMIC_STATE_DEPTH_BIAS})
                       .withSimpleColorBlendState(vk::PipelineParameters::ColorBlendBehavior::None)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::ShadowMapFactory>()
                       .addDescriptorSet<ds::Object3DFactory>()
                       .build());

    createPipeline(
        Config::PipelineIds::PointShadowMapRegular,
        vk::PipelineParameters()
            .withShader(Config::ShaderIds::PointShadowVertex, VK_SHADER_STAGE_VERTEX_BIT)
            .withShader(Config::ShaderIds::PointShadowGeometry, VK_SHADER_STAGE_GEOMETRY_BIT)
            .withShader(Config::ShaderIds::PointShadowFragment, VK_SHADER_STAGE_FRAGMENT_BIT)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withVertexFormat(prim::Vertex3D::bindingDescription(),
                              prim::Vertex3D::attributeDescriptions())
            .withRasterizer(rasterizerShadow)
            .withEnableDepthClipping()
            .addDynamicStates({VK_DYNAMIC_STATE_DEPTH_BIAS})
            .withSimpleColorBlendState(vk::PipelineParameters::ColorBlendBehavior::None)
            .withDepthStencilState(&depthStencilDepthEnabled)
            .addDescriptorSet<ds::ShadowMapFactory>()
            .addDescriptorSet<ds::Object3DFactory>()
            .build());

    createPipeline(
        Config::PipelineIds::PointShadowMapSkinned,
        vk::PipelineParameters()
            .withShader(Config::ShaderIds::PointShadowVertex, VK_SHADER_STAGE_VERTEX_BIT)
            .withShader(Config::ShaderIds::PointShadowGeometry, VK_SHADER_STAGE_GEOMETRY_BIT)
            .withShader(Config::ShaderIds::PointShadowFragment, VK_SHADER_STAGE_FRAGMENT_BIT)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withVertexFormat(prim::Vertex3DSkinned::bindingDescription(),
                              prim::Vertex3DSkinned::attributeDescriptions())
            .withRasterizer(rasterizerShadow)
            .withEnableDepthClipping()
            .addDynamicStates({VK_DYNAMIC_STATE_DEPTH_BIAS})
            .withSimpleColorBlendState(vk::PipelineParameters::ColorBlendBehavior::None)
            .withDepthStencilState(&depthStencilDepthEnabled)
            .addDescriptorSet<ds::ShadowMapFactory>()
            .addDescriptorSet<ds::Object3DFactory>()
            .build());

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    createPipeline(Config::PipelineIds::LitSkinned2DGeometry,
                   vk::PipelineParameters()
                       .withShaders(Config::ShaderIds::Vertex2DSkinned,
                                    Config::ShaderIds::Fragment2DSkinnedLit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withRasterizer(rasterizer)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene2DFactory>()
                       .addDescriptorSet<ds::Object2DFactory>()
                       .build());

    createPipeline(Config::PipelineIds::UnlitSkinned2DGeometry,
                   vk::PipelineParameters()
                       .withShaders(Config::ShaderIds::Vertex2DSkinned,
                                    Config::ShaderIds::Fragment2DSkinnedUnlit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withRasterizer(rasterizer)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene2DFactory>()
                       .addDescriptorSet<ds::Object2DFactory>()
                       .build());

    createPipeline(Config::PipelineIds::Lit2DGeometry,
                   vk::PipelineParameters()
                       .withShaders(Config::ShaderIds::Vertex2D, Config::ShaderIds::Fragment2DLit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withRasterizer(rasterizer)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene2DFactory>()
                       .addDescriptorSet<ds::Object2DFactory>()
                       .build());

    createPipeline(Config::PipelineIds::Unlit2DGeometry,
                   vk::PipelineParameters()
                       .withShaders(Config::ShaderIds::Vertex2D, Config::ShaderIds::Fragment2DUnlit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withRasterizer(rasterizer)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene2DFactory>()
                       .addDescriptorSet<ds::Object2DFactory>()
                       .build());

    createPipeline(Config::PipelineIds::Lines2D,
                   vk::PipelineParameters()
                       .withShaders(Config::ShaderIds::Vertex2D, Config::ShaderIds::Fragment2DUnlit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_LINE_LIST)
                       .withRasterizer(rasterizer)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene2DFactory>()
                       .addDescriptorSet<ds::Object2DFactory>()
                       .build());

    createPipeline(Config::PipelineIds::Unlit2DGeometryNoDepthWrite,
                   vk::PipelineParameters()
                       .withShaders(Config::ShaderIds::Vertex2D, Config::ShaderIds::Fragment2DUnlit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withRasterizer(rasterizer)
                       .withDepthStencilState(&depthStencilDepthWriteDisabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene2DFactory>()
                       .addDescriptorSet<ds::Object2DFactory>()
                       .build());

    createPipeline(
        Config::PipelineIds::Text,
        vk::PipelineParameters()
            .withShaders(Config::ShaderIds::Vertex2DSkinned, Config::ShaderIds::TextFragment)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withRasterizer(rasterizer)
            .withDepthStencilState(&depthStencilDepthEnabled)
            .addDescriptorSet<ds::GlobalDataFactory>()
            .addDescriptorSet<ds::Scene2DFactory>()
            .addDescriptorSet<ds::Object2DFactory>()
            .build());

    createPipeline(
        Config::PipelineIds::SlideshowLit,
        vk::PipelineParameters()
            .withShaders(Config::ShaderIds::SlideshowVert, Config::ShaderIds::Fragment2DSkinnedLit)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withRasterizer(rasterizer)
            .withVertexFormat(prim::SlideshowVertex::bindingDescription(),
                              prim::SlideshowVertex::attributeDescriptions())
            .withDepthStencilState(&depthStencilDepthEnabled)
            .addDescriptorSet<ds::GlobalDataFactory>()
            .addDescriptorSet<ds::Scene2DFactory>()
            .addDescriptorSet<ds::Object2DFactory>()
            .addDescriptorSet<ds::SlideshowFactory>()
            .build());

    createPipeline(Config::PipelineIds::SlideshowUnlit,
                   vk::PipelineParameters()
                       .withShaders(Config::ShaderIds::SlideshowVert,
                                    Config::ShaderIds::Fragment2DSkinnedUnlit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withRasterizer(rasterizer)
                       .withVertexFormat(prim::SlideshowVertex::bindingDescription(),
                                         prim::SlideshowVertex::attributeDescriptions())
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene2DFactory>()
                       .addDescriptorSet<ds::Object2DFactory>()
                       .addDescriptorSet<ds::SlideshowFactory>()
                       .build());

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    createPipeline(
        Config::PipelineIds::FadeEffect,
        vk::PipelineParameters()
            .withShaders(Config::ShaderIds::EmptyVertex, Config::ShaderIds::FadeEffectFragment)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withRasterizer(rasterizer)
            .withDepthStencilState(&depthStencilDepthDisabled)
            .addDescriptorSet<ds::ColorAttachmentFactory>()
            .addPushConstantRange(0, sizeof(float), VK_SHADER_STAGE_FRAGMENT_BIT)
            .build());

    createPipeline(
        Config::PipelineIds::PostProcess3D,
        vk::PipelineParameters()
            .withShaders(Config::ShaderIds::EmptyVertex, Config::ShaderIds::PostProcess3DFragment)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withRasterizer(rasterizer)
            .withDepthStencilState(&depthStencilDepthDisabled)
            .addDescriptorSet<ds::ColorAttachmentFactory>()
            .addDescriptorSet<ds::ColorAttachmentFactory>()
            .addDescriptorSet<ds::GlobalDataFactory>()
            .build());

    constexpr std::uint32_t BloomPcSize =
        sizeof(float) * (Settings::MaxBloomFilterSize + 1) + sizeof(std::uint32_t) * 2;
    createPipeline(Config::PipelineIds::BloomHighlightFilter,
                   vk::PipelineParameters()
                       .withShaders(Config::ShaderIds::EmptyVertex,
                                    Config::ShaderIds::BloomHighlightFilterFragment)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withRasterizer(rasterizer)
                       .withDepthStencilState(&depthStencilDepthDisabled)
                       .addDescriptorSet<ds::ColorAttachmentFactory>()
                       .addPushConstantRange(0, BloomPcSize, VK_SHADER_STAGE_FRAGMENT_BIT)
                       .build());
    createPipeline(
        Config::PipelineIds::BloomBlur,
        vk::PipelineParameters()
            .withShaders(Config::ShaderIds::EmptyVertex, Config::ShaderIds::BloomBlurFragment)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withRasterizer(rasterizer)
            .withDepthStencilState(&depthStencilDepthDisabled)
            .addDescriptorSet<ds::ColorAttachmentFactory>()
            .addPushConstantRange(0, BloomPcSize, VK_SHADER_STAGE_FRAGMENT_BIT)
            .build());
}

} // namespace res
} // namespace rc
} // namespace bl
