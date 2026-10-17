#include <BLIB/Objects/2D/BatchIcon.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
BatchIcon::BatchIcon(Type type, const glm::vec2& size)
: IconBase(type, size) {}

void BatchIcon::create(engine::Engine& engine, BatchedShapes& owner) {
    BatchShape::create(engine, owner);
}

} // namespace o2d
} // namespace obj
} // namespace bl
