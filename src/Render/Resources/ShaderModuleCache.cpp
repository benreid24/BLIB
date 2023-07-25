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

    if (path[0] <= static_cast<int>(Config::ShaderIds::MaxId)) {
        switch (path[0]) {
        case Config::ShaderIds::EmptyVertex[0]:
            return loadShader(BUILTIN_SHADER("empty.vert.spv"));

        case Config::ShaderIds::MeshVertex[0]:
            return loadShader(BUILTIN_SHADER("mesh.vert.spv"));
        case Config::ShaderIds::MeshFragment[0]:
            return loadShader(BUILTIN_SHADER("mesh.frag.spv"));
        case Config::ShaderIds::SkinnedMeshVertex[0]:
            return loadShader(BUILTIN_SHADER("skinnedMesh.vert.spv"));
        case Config::ShaderIds::SkinnedMeshFragment[0]:
            return loadShader(BUILTIN_SHADER("skinnedMesh.frag.spv"));

        case Config::ShaderIds::Lit2DVertex[0]:
            return loadShader(BUILTIN_SHADER("2dlit.vert.spv"));
        case Config::ShaderIds::Lit2DFragment[0]:
            return loadShader(BUILTIN_SHADER("2dlit.frag.spv"));
        case Config::ShaderIds::LitSkinned2DVertex[0]:
            return loadShader(BUILTIN_SHADER("skinned2dlit.vert.spv"));
        case Config::ShaderIds::LitSkinned2DFragment[0]:
            return loadShader(BUILTIN_SHADER("skinned2dlit.frag.spv"));

        case Config::ShaderIds::Unlit2DVertex[0]:
            return loadShader(BUILTIN_SHADER("2d.vert.spv"));
        case Config::ShaderIds::Unlit2DFragment[0]:
            return loadShader(BUILTIN_SHADER("2d.frag.spv"));
        case Config::ShaderIds::UnlitSkinned2DVertex[0]:
            return loadShader(BUILTIN_SHADER("skinned2d.vert.spv"));
        case Config::ShaderIds::UnlitSkinned2DFragment[0]:
            return loadShader(BUILTIN_SHADER("skinned2d.frag.spv"));
        case Config::ShaderIds::TextFragment[0]:
            return loadShader(BUILTIN_SHADER("text.frag.spv"));

        case Config::ShaderIds::FadeEffectFragment[0]:
            return loadShader(BUILTIN_SHADER("fadeEffect.frag.spv"));
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
