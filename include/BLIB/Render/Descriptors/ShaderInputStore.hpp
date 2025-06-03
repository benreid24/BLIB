#ifndef BLIB_RENDER_DESCRIPTORS_SHADERINPUTSTORE_HPP
#define BLIB_RENDER_DESCRIPTORS_SHADERINPUTSTORE_HPP

#include <BLIB/Render/Descriptors/EntityComponentShaderInput.hpp>
#include <BLIB/Render/Descriptors/ShaderInput.hpp>
#include <BLIB/Render/Scenes/Key.hpp>
#include <memory>
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
namespace ds
{
/**
 * @brief Cache for shader inputs for descriptor sets. Shader inputs are instantiated
 *        once and shared across descriptor sets
 *
 * @ingroup Renderer
 */
class ShaderInputStore {
public:
    /**
     * @brief Creates a new shader input store
     *
     * @param engine The game engine instance
     * @param entityCb Callback to map scene id to ECS id
     */
    ShaderInputStore(engine::Engine& engine, const scene::MapKeyToEntityCb& entityCb);

    /**
     * @brief Release resources
     */
    ~ShaderInputStore();

    /**
     * @brief Gets or creates the requested shader input
     *
     * @tparam TInput The type of shader input to fetch or create
     * @tparam args Argument types to the shader input constructor
     * @param args Arguments to the shader input constructor
     * @return A pointer to the shader input to use
     */
    template<typename TInput, typename... TArgs>
    TInput* getShaderInput(TArgs&&... args);

    /**
     * @brief Syncs modified descriptor values in the contained storage modules
     */
    void syncDescriptors();

    /**
     * @brief Copies descriptor data from source (such asECS components) into local buffers
     */
    void copyFromECS();

private:
    engine::Engine& engine;
    const scene::MapKeyToEntityCb entityCb;
    std::unordered_map<std::type_index, std::unique_ptr<ShaderInput>> cache;

    void initInput(ShaderInput& input);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TInput, typename... TArgs>
TInput* ShaderInputStore::getShaderInput(TArgs&&... args) {
    static_assert(std::is_base_of<ShaderInput, TInput>::value,
                  "TInput must be derived from ShaderInput");

    auto it = cache.find(typeid(TInput));
    if (it == cache.end()) {
        it =
            cache
                .try_emplace(typeid(TInput), std::make_unique<TInput>(std::forward<TArgs>(args)...))
                .first;
        initInput(*it->second);
    }
    return static_cast<TInput*>(it->second.get());
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
