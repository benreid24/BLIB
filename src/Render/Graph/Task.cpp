#include <BLIB/Render/Graph/Task.hpp>

namespace bl
{
namespace rc
{
namespace rg
{
Task::Task()
: output(nullptr) {}

void Task::reset() {
    output = nullptr;
    inputs.clear();
}

} // namespace rg
} // namespace rc
} // namespace bl