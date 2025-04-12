#include <BLIB/Render/Graph/TaskAssets.hpp>

namespace bl
{
namespace rc
{
namespace rg
{
TaskAssets::TaskAssets() {}

void TaskAssets::init(const TaskAssetTags& tags) {
    outputs.clear();
    requiredInputs.clear();
    optionalInputs.clear();

    outputs.resize(tags.outputs.size(), nullptr);
    requiredInputs.resize(tags.requiredInputs.size(), nullptr);
    optionalInputs.resize(tags.optionalInputs.size(), nullptr);
}

} // namespace rg
} // namespace rc
} // namespace bl
