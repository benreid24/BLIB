#include <BLIB/Components/Shape2D.hpp>

namespace bl
{
namespace com
{
void Shape2D::commit() {
    drawParams = indexBuffer.getDrawParameters();
    indexBuffer.queueTransfer();
    if (sceneRef.object) { syncDrawParamsToScene(); }
}

} // namespace com
} // namespace bl
