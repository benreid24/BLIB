#include <BLIB/Render/Graph/GraphAssetPool.hpp>

namespace bl
{
namespace rc
{
namespace rg
{
GraphAssetPool::GraphAssetPool(AssetPool& pool)
: pool(pool) {}

GraphAsset* GraphAssetPool::getAsset(std::string_view tag) {
    //
}

} // namespace rg
} // namespace rc
} // namespace bl
