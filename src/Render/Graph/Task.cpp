#include <BLIB/Render/Graph/Task.hpp>

namespace bl
{
namespace rc
{
namespace rg
{
void Task::prepareInputs(const ExecutionContext& ctx) {
    for (GraphAsset* input : assets.requiredInputs) { input->asset->prepareForInput(ctx); }
    for (GraphAsset* input : assets.optionalInputs) {
        if (input) { input->asset->prepareForInput(ctx); }
    }
}

void Task::update(float) {}

} // namespace rg
} // namespace rc
} // namespace bl
