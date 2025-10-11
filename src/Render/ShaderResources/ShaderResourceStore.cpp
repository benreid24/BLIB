#include <BLIB/Render/ShaderResources/ShaderResourceStore.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace rc
{
namespace sr
{
ShaderResourceStore::ShaderResourceStore(engine::Engine& engine, Scene& owner,
                                         const scene::MapKeyToEntityCb& entityCb)
: engine(engine)
, owner(owner)
, entityCb(entityCb) {}

ShaderResourceStore::~ShaderResourceStore() {
    for (auto& pair : cache) { pair.second->cleanup(); }
}

void ShaderResourceStore::syncDescriptors() {
    for (auto& pair : cache) { pair.second->performGpuSync(); }
}

void ShaderResourceStore::copyFromECS() {
    for (auto& pair : cache) { pair.second->copyFromSource(); }
}

void ShaderResourceStore::initInput(ShaderResource& input) {
    input.init(engine, engine.renderer().vulkanState(), owner, entityCb);
}

} // namespace sr
} // namespace rc
} // namespace bl
