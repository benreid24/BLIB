#include <BLIB/Render/Resources/PipelineCache.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Config/PipelineIds.hpp>
#include <BLIB/Render/Config/ShaderIds.hpp>
#include <BLIB/Render/Config/Specializations3D.hpp>
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

    vk::PipelineSpecialization lightingDisabledSpecialization;
    lightingDisabledSpecialization
        .createShaderSpecializations(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(std::uint32_t), 1)
        .setShaderSpecializationValue<std::uint32_t>(VK_SHADER_STAGE_FRAGMENT_BIT, 0, 0, 0);

    createPipeline(cfg::PipelineIds::Mesh3D,
                   vk::PipelineParameters()
                       .withShaders(cfg::ShaderIds::MeshVertex, cfg::ShaderIds::MeshFragment)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withVertexFormat(prim::Vertex3D::bindingDescription(),
                                         prim::Vertex3D::attributeDescriptions())
                       .withRasterizer(rasterizer3d)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene3DFactory>()
                       .addDescriptorSet<ds::Object3DFactory>()
                       .withDeclareSpecializations(1)
                       .withSpecialization(cfg::Specializations3D::LightingDisabled,
                                           lightingDisabledSpecialization)
                       .build());

    createPipeline(cfg::PipelineIds::LitMesh3DMaterial,
                   vk::PipelineParameters()
                       .withShaders(cfg::ShaderIds::MeshVertexMaterial,
                                    cfg::ShaderIds::MeshFragmentMaterialLit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withVertexFormat(prim::Vertex3D::bindingDescription(),
                                         prim::Vertex3D::attributeDescriptions())
                       .withRasterizer(rasterizer3d)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene3DFactory>()
                       .addDescriptorSet<ds::Object3DFactory>()
                       .build());

    createPipeline(cfg::PipelineIds::UnlitMesh3DMaterial,
                   vk::PipelineParameters()
                       .withShaders(cfg::ShaderIds::MeshVertexMaterial,
                                    cfg::ShaderIds::MeshFragmentMaterialUnlit)
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
        cfg::PipelineIds::LitMesh3DSkinned,
        vk::PipelineParameters()
            .withShaders(cfg::ShaderIds::MeshVertexSkinned, cfg::ShaderIds::MeshFragmentSkinnedLit)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withVertexFormat(prim::Vertex3D::bindingDescription(),
                              prim::Vertex3D::attributeDescriptions())
            .withRasterizer(rasterizer3d)
            .withDepthStencilState(&depthStencilDepthEnabled)
            .addDescriptorSet<ds::GlobalDataFactory>()
            .addDescriptorSet<ds::Scene3DFactory>()
            .addDescriptorSet<ds::Object3DFactory>()
            .build());

    createPipeline(cfg::PipelineIds::UnlitMesh3DSkinned,
                   vk::PipelineParameters()
                       .withShaders(cfg::ShaderIds::MeshVertexSkinned,
                                    cfg::ShaderIds::MeshFragmentSkinnedUnlit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withVertexFormat(prim::Vertex3D::bindingDescription(),
                                         prim::Vertex3D::attributeDescriptions())
                       .withRasterizer(rasterizer3d)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene3DFactory>()
                       .addDescriptorSet<ds::Object3DFactory>()
                       .build());

    createPipeline(cfg::PipelineIds::Skybox,
                   vk::PipelineParameters()
                       .withShaders(cfg::ShaderIds::SkyboxVertex, cfg::ShaderIds::SkyboxFragment)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withVertexFormat(prim::Vertex3D::bindingDescription(),
                                         prim::Vertex3D::attributeDescriptions())
                       .withRasterizer(skyboxRasterizer)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene3DFactory>()
                       .addDescriptorSet<ds::Object3DFactory>()
                       .build());

    createPipeline(cfg::PipelineIds::ShadowMapRegular,
                   vk::PipelineParameters()
                       .withShader(cfg::ShaderIds::ShadowVertex, VK_SHADER_STAGE_VERTEX_BIT)
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

    createPipeline(cfg::PipelineIds::ShadowMapSkinned,
                   vk::PipelineParameters()
                       .withShader(cfg::ShaderIds::ShadowVertex, VK_SHADER_STAGE_VERTEX_BIT)
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
        cfg::PipelineIds::PointShadowMapRegular,
        vk::PipelineParameters()
            .withShader(cfg::ShaderIds::PointShadowVertex, VK_SHADER_STAGE_VERTEX_BIT)
            .withShader(cfg::ShaderIds::PointShadowGeometry, VK_SHADER_STAGE_GEOMETRY_BIT)
            .withShader(cfg::ShaderIds::PointShadowFragment, VK_SHADER_STAGE_FRAGMENT_BIT)
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
        cfg::PipelineIds::PointShadowMapSkinned,
        vk::PipelineParameters()
            .withShader(cfg::ShaderIds::PointShadowVertex, VK_SHADER_STAGE_VERTEX_BIT)
            .withShader(cfg::ShaderIds::PointShadowGeometry, VK_SHADER_STAGE_GEOMETRY_BIT)
            .withShader(cfg::ShaderIds::PointShadowFragment, VK_SHADER_STAGE_FRAGMENT_BIT)
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

    createPipeline(
        cfg::PipelineIds::LitSkinned2DGeometry,
        vk::PipelineParameters()
            .withShaders(cfg::ShaderIds::Vertex2DSkinned, cfg::ShaderIds::Fragment2DSkinnedLit)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withRasterizer(rasterizer)
            .withDepthStencilState(&depthStencilDepthEnabled)
            .addDescriptorSet<ds::GlobalDataFactory>()
            .addDescriptorSet<ds::Scene2DFactory>()
            .addDescriptorSet<ds::Object2DFactory>()
            .build());

    createPipeline(
        cfg::PipelineIds::UnlitSkinned2DGeometry,
        vk::PipelineParameters()
            .withShaders(cfg::ShaderIds::Vertex2DSkinned, cfg::ShaderIds::Fragment2DSkinnedUnlit)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withRasterizer(rasterizer)
            .withDepthStencilState(&depthStencilDepthEnabled)
            .addDescriptorSet<ds::GlobalDataFactory>()
            .addDescriptorSet<ds::Scene2DFactory>()
            .addDescriptorSet<ds::Object2DFactory>()
            .build());

    createPipeline(cfg::PipelineIds::Lit2DGeometry,
                   vk::PipelineParameters()
                       .withShaders(cfg::ShaderIds::Vertex2D, cfg::ShaderIds::Fragment2DLit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withRasterizer(rasterizer)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene2DFactory>()
                       .addDescriptorSet<ds::Object2DFactory>()
                       .build());

    createPipeline(cfg::PipelineIds::Unlit2DGeometry,
                   vk::PipelineParameters()
                       .withShaders(cfg::ShaderIds::Vertex2D, cfg::ShaderIds::Fragment2DUnlit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withRasterizer(rasterizer)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene2DFactory>()
                       .addDescriptorSet<ds::Object2DFactory>()
                       .build());

    createPipeline(cfg::PipelineIds::Lines2D,
                   vk::PipelineParameters()
                       .withShaders(cfg::ShaderIds::Vertex2D, cfg::ShaderIds::Fragment2DUnlit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_LINE_LIST)
                       .withRasterizer(rasterizer)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene2DFactory>()
                       .addDescriptorSet<ds::Object2DFactory>()
                       .build());

    createPipeline(cfg::PipelineIds::Unlit2DGeometryNoDepthWrite,
                   vk::PipelineParameters()
                       .withShaders(cfg::ShaderIds::Vertex2D, cfg::ShaderIds::Fragment2DUnlit)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withRasterizer(rasterizer)
                       .withDepthStencilState(&depthStencilDepthWriteDisabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene2DFactory>()
                       .addDescriptorSet<ds::Object2DFactory>()
                       .build());

    createPipeline(cfg::PipelineIds::Text,
                   vk::PipelineParameters()
                       .withShaders(cfg::ShaderIds::Vertex2DSkinned, cfg::ShaderIds::TextFragment)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withRasterizer(rasterizer)
                       .withDepthStencilState(&depthStencilDepthEnabled)
                       .addDescriptorSet<ds::GlobalDataFactory>()
                       .addDescriptorSet<ds::Scene2DFactory>()
                       .addDescriptorSet<ds::Object2DFactory>()
                       .build());

    createPipeline(
        cfg::PipelineIds::SlideshowLit,
        vk::PipelineParameters()
            .withShaders(cfg::ShaderIds::SlideshowVert, cfg::ShaderIds::Fragment2DSkinnedLit)
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

    createPipeline(
        cfg::PipelineIds::SlideshowUnlit,
        vk::PipelineParameters()
            .withShaders(cfg::ShaderIds::SlideshowVert, cfg::ShaderIds::Fragment2DSkinnedUnlit)
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

    createPipeline(cfg::PipelineIds::FadeEffect,
                   vk::PipelineParameters()
                       .withShaders(cfg::ShaderIds::EmptyVertex, cfg::ShaderIds::FadeEffectFragment)
                       .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                       .withRasterizer(rasterizer)
                       .withDepthStencilState(&depthStencilDepthDisabled)
                       .addDescriptorSet<ds::ColorAttachmentFactory>()
                       .addPushConstantRange(0, sizeof(float), VK_SHADER_STAGE_FRAGMENT_BIT)
                       .build());

    createPipeline(
        cfg::PipelineIds::PostProcess3D,
        vk::PipelineParameters()
            .withShaders(cfg::ShaderIds::EmptyVertex, cfg::ShaderIds::PostProcess3DFragment)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withRasterizer(rasterizer)
            .withDepthStencilState(&depthStencilDepthDisabled)
            .addDescriptorSet<ds::ColorAttachmentFactory>()
            .addDescriptorSet<ds::ColorAttachmentFactory>()
            .addDescriptorSet<ds::GlobalDataFactory>()
            .build());

    constexpr std::uint32_t BloomPcSize =
        sizeof(float) * (Settings::MaxBloomFilterSize + 1) + sizeof(std::uint32_t) * 2;
    createPipeline(
        cfg::PipelineIds::BloomHighlightFilter,
        vk::PipelineParameters()
            .withShaders(cfg::ShaderIds::EmptyVertex, cfg::ShaderIds::BloomHighlightFilterFragment)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withRasterizer(rasterizer)
            .withDepthStencilState(&depthStencilDepthDisabled)
            .addDescriptorSet<ds::ColorAttachmentFactory>()
            .addPushConstantRange(0, BloomPcSize, VK_SHADER_STAGE_FRAGMENT_BIT)
            .build());
    createPipeline(cfg::PipelineIds::BloomBlur,
                   vk::PipelineParameters()
                       .withShaders(cfg::ShaderIds::EmptyVertex, cfg::ShaderIds::BloomBlurFragment)
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
