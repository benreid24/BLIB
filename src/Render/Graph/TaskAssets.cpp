#include <BLIB/Render/Graph/TaskAssets.hpp>

namespace bl
{
namespace rc
{
namespace rg
{
TaskAssets::TaskAssets()
: outputTag()
, output(nullptr) {}

void TaskAssets::init(const TaskAssetTags& tags) {
    outputTag = {};
    output    = nullptr;
    requiredInputs.resize(tags.requiredInputs.size(), nullptr);
    optionalInputs.resize(tags.optionalInputs.size(), nullptr);
}

} // namespace rg
} // namespace rc
} // namespace bl
