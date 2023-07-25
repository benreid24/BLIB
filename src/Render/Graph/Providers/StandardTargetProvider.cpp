#include <BLIB/Render/Graph/Providers/StandardTargetProvider.hpp>

#include <BLIB/Render/Graph/Assets/StandardTargetAsset.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
rg::Asset* StandardAssetProvider::create() { return new StandardTargetAsset(); }

} // namespace rgi
} // namespace rc
} // namespace bl
