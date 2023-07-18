#include <BLIB/Render/Graph/AssetFactory.hpp>

#include <stdexcept>

namespace bl
{
namespace rc
{
namespace rg
{
Asset* AssetFactory::createAsset(std::string_view tag) {
    const auto it = providers.find(tag);
    if (it == providers.end()) { throw std::runtime_error("Missing provider for asset tag"); }
    return it->second->create();
}

} // namespace rg
} // namespace rc
} // namespace bl
