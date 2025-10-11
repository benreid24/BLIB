#ifndef BLIB_RENDER_SHADERRESOURCES_SHADERRESOURCESTORE_HPP
#define BLIB_RENDER_SHADERRESOURCES_SHADERRESOURCESTORE_HPP

#include <BLIB/Render/Scenes/Key.hpp>
#include <BLIB/Render/ShaderResources/ShaderResource.hpp>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace bl
{
namespace engine
{
class Engine;
}

namespace rc
{
class RenderTarget;

namespace sr
{
/**
 * @brief Cache for shader inputs for descriptor sets. Shader inputs are instantiated
 *        once and shared across descriptor sets
 *
 * @ingroup Renderer
 */
class ShaderResourceStore {
public:
    /**
     * @brief Creates a new shader input store
     *
     * @param engine The game engine instance
     */
    ShaderResourceStore(engine::Engine& engine);

    /**
     * @brief Release resources
     */
    ~ShaderResourceStore();

    /**
     * @brief Gets or creates the requested shader input using the type as the id
     *
     * @tparam TInput The type of shader input to fetch or create
     * @tparam args Argument types to the shader input constructor
     * @param args Arguments to the shader input constructor
     * @return A pointer to the shader input to use
     */
    template<typename TInput, typename... TArgs>
    TInput* getShaderInput(TArgs&&... args);

    /**
     * @brief Gets or creates the requested shader input using an explicit id
     *
     * @tparam TInput The type of shader input to fetch or create
     * @tparam args Argument types to the shader input constructor
     * @param id The id of the shader input to fetch or create
     * @param args Arguments to the shader input constructor
     * @return A pointer to the shader input to use
     */
    template<typename TInput, typename... TArgs>
    TInput* getShaderInputWithId(const std::string& id, TArgs&&... args);

    /**
     * @brief Syncs modified descriptor values in the contained storage modules
     */
    void performTransfers();

    /**
     * @brief Copies descriptor data from source (such asECS components) into local buffers
     */
    void updateFromSources();

private:
    engine::Engine& engine;
    std::unordered_map<std::string, std::unique_ptr<ShaderResource>> cache;

    void initInput(ShaderResource& input);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TInput, typename... TArgs>
TInput* ShaderResourceStore::getShaderInput(TArgs&&... args) {
    return getShaderInputWithId<TInput>(typeid(TInput).name(), std::forward<TArgs>(args)...);
}

template<typename TInput, typename... TArgs>
TInput* ShaderResourceStore::getShaderInputWithId(const std::string& id, TArgs&&... args) {
    static_assert(std::is_base_of<ShaderResource, TInput>::value,
                  "TInput must be derived from ShaderResource");

    auto it = cache.find(id);
    if (it == cache.end()) {
        it = cache.try_emplace(id, std::make_unique<TInput>(std::forward<TArgs>(args)...)).first;
        initInput(*it->second);
    }
    return static_cast<TInput*>(it->second.get());
}

} // namespace sr
} // namespace rc
} // namespace bl

#endif
