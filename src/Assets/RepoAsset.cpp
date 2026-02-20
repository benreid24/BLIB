#include <BLIB/Assets/RepoAsset.hpp>

namespace bl
{
namespace as
{
RepoAsset::RepoAsset(Repository& repo)
: asset(repo)
, refCount(0) {}

} // namespace as
} // namespace bl
