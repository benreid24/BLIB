#include <BLIB/Components/BatchedShapes2D.hpp>

namespace bl
{
namespace com
{
void BatchedShapes2D::commit() {
    drawParams = indexBuffer.getDrawParameters();
    if (sceneRef.object) { syncDrawParamsToScene(); }
}

} // namespace com
} // namespace bl
