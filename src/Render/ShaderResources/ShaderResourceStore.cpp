#include <BLIB/Render/ShaderResources/ShaderResourceStore.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace rc
{
namespace sr
{
ShaderResourceStore::ShaderResourceStore(engine::Engine& engine)
: engine(engine) {}

ShaderResourceStore::~ShaderResourceStore() {
    for (auto& pair : cache) { pair.second->cleanup(); }
}

void ShaderResourceStore::performTransfers() {
    for (auto& pair : cache) { pair.second->performTransfer(); }
}

void ShaderResourceStore::updateFromSources() {
    for (auto& pair : cache) { pair.second->copyFromSource(); }
}

void ShaderResourceStore::initInput(ShaderResource& input) { input.init(engine); }

} // namespace sr
} // namespace rc
} // namespace bl
