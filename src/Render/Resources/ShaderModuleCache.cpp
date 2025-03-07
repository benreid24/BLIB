#include <BLIB/Render/Resources/ShaderModuleCache.hpp>

#include <BLIB/Render/Config.hpp>
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
        case Config::ShaderIds::EmptyVertex[0]:
            return loadShader(BUILTIN_SHADER("empty.vert.spv"));

        case Config::ShaderIds::MeshVertex[0]:
            return loadShader(BUILTIN_SHADER("3D/mesh.vert.spv"));
        case Config::ShaderIds::MeshFragmentLit[0]:
            return loadShader(BUILTIN_SHADER("3D/meshLit.frag.spv"));
        case Config::ShaderIds::MeshFragmentUnlit[0]:
            return loadShader(BUILTIN_SHADER("3D/meshUnlit.frag.spv"));

        case Config::ShaderIds::MeshVertexMaterial[0]:
            return loadShader(BUILTIN_SHADER("3D/meshMaterial.vert.spv"));
        case Config::ShaderIds::MeshFragmentMaterialLit[0]:
            return loadShader(BUILTIN_SHADER("3D/meshMaterialLit.frag.spv"));
        case Config::ShaderIds::MeshFragmentMaterialUnlit[0]:
            return loadShader(BUILTIN_SHADER("3D/meshMaterialUnlit.frag.spv"));

        case Config::ShaderIds::MeshVertexSkinned[0]:
            return loadShader(BUILTIN_SHADER("3D/meshSkinned.vert.spv"));

        case Config::ShaderIds::Vertex2D[0]:
            return loadShader(BUILTIN_SHADER("2D/2d.vert.spv"));
        case Config::ShaderIds::Fragment2DLit[0]:
            return loadShader(BUILTIN_SHADER("2D/2dlit.frag.spv"));
        case Config::ShaderIds::Fragment2DUnlit[0]:
            return loadShader(BUILTIN_SHADER("2D/2d.frag.spv"));

        case Config::ShaderIds::Vertex2DSkinned[0]:
            return loadShader(BUILTIN_SHADER("2D/skinned2d.vert.spv"));
        case Config::ShaderIds::Fragment2DSkinnedLit[0]:
            return loadShader(BUILTIN_SHADER("2D/skinned2dlit.frag.spv"));
        case Config::ShaderIds::Fragment2DSkinnedUnlit[0]:
            return loadShader(BUILTIN_SHADER("2D/skinned2d.frag.spv"));
        case Config::ShaderIds::Fragment2DRotatedParticle[0]:
            return loadShader(BUILTIN_SHADER("2D/rotatedParticle.frag.spv"));

        case Config::ShaderIds::TextFragment[0]:
            return loadShader(BUILTIN_SHADER("2D/text.frag.spv"));
        case Config::ShaderIds::SlideshowVert[0]:
            return loadShader(BUILTIN_SHADER("2D/slideshow.vert.spv"));

        case Config::ShaderIds::FadeEffectFragment[0]:
            return loadShader(BUILTIN_SHADER("PostFX/fadeEffect.frag.spv"));
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
    if (u32data[0] != Config::SPIRVMagicNumber) {
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
