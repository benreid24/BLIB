#include <BLIB/Render/Resources/ShaderModuleCache.hpp>

#include <BLIB/Render/Config/Constants.hpp>
#include <BLIB/Render/Config/ShaderIds.hpp>
#include <BLIB/Resources.hpp>

#define STRINGIFY_HELPER(X) #X
#define STRINGIFY(x) STRINGIFY_HELPER(x)
#define BUILTIN_SHADER(path) STRINGIFY(SHADER_PATH) "/" path

namespace bl
{
namespace rc
{
namespace res
{
VkShaderModule ShaderModuleCache::loadShader(const std::string& path) {
    const auto it = cache.find(path);
    if (it != cache.end()) { return it->second; }

    char* data      = nullptr;
    std::size_t len = 0;

    if (path[1] == '\0') {
        switch (path[0]) {
        case cfg::ShaderIds::EmptyVertex[0]:
            return loadShader(BUILTIN_SHADER("empty.vert.spv"));

        case cfg::ShaderIds::MeshVertex[0]:
            return loadShader(BUILTIN_SHADER("3D/mesh.vert.spv"));
        case cfg::ShaderIds::MeshFragmentLit[0]:
            return loadShader(BUILTIN_SHADER("3D/Forward/meshLit.frag.spv"));
        case cfg::ShaderIds::MeshFragmentUnlit[0]:
            return loadShader(BUILTIN_SHADER("3D/Forward/meshUnlit.frag.spv"));

        case cfg::ShaderIds::MeshVertexMaterial[0]:
            return loadShader(BUILTIN_SHADER("3D/meshMaterial.vert.spv"));
        case cfg::ShaderIds::MeshFragmentMaterialLit[0]:
            return loadShader(BUILTIN_SHADER("3D/Forward/meshMaterialLit.frag.spv"));
        case cfg::ShaderIds::MeshFragmentMaterialUnlit[0]:
            return loadShader(BUILTIN_SHADER("3D/Forward/meshMaterialUnlit.frag.spv"));

        case cfg::ShaderIds::MeshVertexSkinned[0]:
            return loadShader(BUILTIN_SHADER("3D/meshSkinned.vert.spv"));

        case cfg::ShaderIds::SkyboxVertex[0]:
            return loadShader(BUILTIN_SHADER("3D/skybox.vert.spv"));
        case cfg::ShaderIds::SkyboxFragment[0]:
            return loadShader(BUILTIN_SHADER("3D/skybox.frag.spv"));

        case cfg::ShaderIds::ShadowVertex[0]:
            return loadShader(BUILTIN_SHADER("3D/Shadows/shadowMap.vert.spv"));
        case cfg::ShaderIds::PointShadowVertex[0]:
            return loadShader(BUILTIN_SHADER("3D/Shadows/pointShadowMap.vert.spv"));
        case cfg::ShaderIds::PointShadowGeometry[0]:
            return loadShader(BUILTIN_SHADER("3D/Shadows/pointShadowMap.geom.spv"));
        case cfg::ShaderIds::PointShadowFragment[0]:
            return loadShader(BUILTIN_SHADER("3D/Shadows/pointShadowMap.frag.spv"));

        case cfg::ShaderIds::Vertex2D[0]:
            return loadShader(BUILTIN_SHADER("2D/2d.vert.spv"));
        case cfg::ShaderIds::Fragment2DLit[0]:
            return loadShader(BUILTIN_SHADER("2D/2dlit.frag.spv"));
        case cfg::ShaderIds::Fragment2DUnlit[0]:
            return loadShader(BUILTIN_SHADER("2D/2d.frag.spv"));

        case cfg::ShaderIds::Vertex2DSkinned[0]:
            return loadShader(BUILTIN_SHADER("2D/skinned2d.vert.spv"));
        case cfg::ShaderIds::Fragment2DSkinnedLit[0]:
            return loadShader(BUILTIN_SHADER("2D/skinned2dlit.frag.spv"));
        case cfg::ShaderIds::Fragment2DSkinnedUnlit[0]:
            return loadShader(BUILTIN_SHADER("2D/skinned2d.frag.spv"));
        case cfg::ShaderIds::Fragment2DRotatedParticle[0]:
            return loadShader(BUILTIN_SHADER("2D/rotatedParticle.frag.spv"));

        case cfg::ShaderIds::TextFragment[0]:
            return loadShader(BUILTIN_SHADER("2D/text.frag.spv"));
        case cfg::ShaderIds::SlideshowVert[0]:
            return loadShader(BUILTIN_SHADER("2D/slideshow.vert.spv"));

        case cfg::ShaderIds::FadeEffectFragment[0]:
            return loadShader(BUILTIN_SHADER("PostFX/fadeEffect.frag.spv"));
        case cfg::ShaderIds::PostProcess3DFragment[0]:
            return loadShader(BUILTIN_SHADER("PostFX/3dPostProcess.frag.spv"));
        case cfg::ShaderIds::BloomBlurFragment[0]:
            return loadShader(BUILTIN_SHADER("PostFX/bloomBlur.frag.spv"));
        case cfg::ShaderIds::BloomHighlightFilterFragment[0]:
            return loadShader(BUILTIN_SHADER("PostFX/bloomHighlightFilter.frag.spv"));

        default:
            BL_LOG_ERROR << "Invalid built-in shader id: " << static_cast<int>(path[0]);
            throw std::runtime_error("Invalid built-in shader id");
        }
    }
    else {
        if (!resource::FileSystem::getData(path, &data, len) || len < 4) {
            BL_LOG_ERROR << "Failed to load shader: " << path;
            throw std::runtime_error("Failed to load shader");
        }
    }

    const std::uint32_t* u32data = reinterpret_cast<const std::uint32_t*>(data);
    if (u32data[0] != cfg::Constants::SPIRVMagicNumber) {
        BL_LOG_ERROR << "Shader '" << path << "' is not compiled";
        throw std::runtime_error("Shader compilation is not supported");
    }

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = len;
    createInfo.pCode    = u32data;
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module");
    }
    cache.try_emplace(path, shaderModule);
    return shaderModule;
}

void ShaderModuleCache::init(VkDevice d) { device = d; }

void ShaderModuleCache::cleanup() {
    for (auto& pair : cache) { vkDestroyShaderModule(device, pair.second, nullptr); }
    cache.clear();
}

} // namespace res
} // namespace rc
} // namespace bl
