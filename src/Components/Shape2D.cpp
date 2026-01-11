#include <BLIB/Components/Shape2D.hpp>

namespace bl
{
namespace com
{
void Shape2D::commit() {
    drawParams = indexBuffer.getDrawParameters();
    indexBuffer.queueTransfer();
}

} // namespace com
} // namespace bl
