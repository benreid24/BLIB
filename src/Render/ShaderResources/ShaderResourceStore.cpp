#include <BLIB/Render/ShaderResources/ShaderResourceStore.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace rc
{
namespace sr
{
ShaderResourceStore::ShaderResourceStore(engine::Engine& engine, RenderTarget& owner)
: engine(engine)
, owner(owner) {}

ShaderResourceStore::~ShaderResourceStore() { cleanup(); }

void ShaderResourceStore::cleanup() {
    for (auto& pair : cache) { pair.second->cleanup(); }
    cache.clear();
}

void ShaderResourceStore::performTransfers() {
    for (auto& pair : cache) { pair.second->performTransfer(); }
}

void ShaderResourceStore::updateFromSources() {
    for (auto& pair : cache) { pair.second->copyFromSource(); }
}

void ShaderResourceStore::initInput(ShaderResource& input) { input.init(engine, owner); }

} // namespace sr
} // namespace rc
} // namespace bl
