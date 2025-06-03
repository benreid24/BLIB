#include <BLIB/Render/Descriptors/ShaderInputStore.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
ShaderInputStore::ShaderInputStore(engine::Engine& engine, const scene::MapKeyToEntityCb& entityCb)
: engine(engine)
, entityCb(entityCb) {}

ShaderInputStore::~ShaderInputStore() {
    for (auto& pair : cache) { pair.second->cleanup(); }
}

void ShaderInputStore::syncDescriptors() {
    for (auto& pair : cache) { pair.second->performGpuSync(); }
}

void ShaderInputStore::copyFromECS() {
    for (auto& pair : cache) { pair.second->copyFromSource(); }
}

void ShaderInputStore::initInput(ShaderInput& input) {
    input.init(engine, engine.renderer().vulkanState(), entityCb);
}

} // namespace ds
} // namespace rc
} // namespace bl
