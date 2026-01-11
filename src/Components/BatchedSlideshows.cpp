#include <BLIB/Components/BatchedSlideshows.hpp>

namespace bl
{
namespace com
{
BatchedSlideshows::BatchedSlideshows(rc::Renderer& renderer, unsigned int ic) {
    create(renderer, ic);
}

void BatchedSlideshows::create(rc::Renderer& renderer, unsigned int ic) {
    indexBuffer.create(renderer, ic * 4, ic * 6);
    drawParams = indexBuffer.getDrawParameters();
}

void BatchedSlideshows::updateDrawParams() {
    if (indexBuffer.getIndexBuffer().vertexCount() > 0) {
        drawParams = indexBuffer.getDrawParameters();
    }
}

} // namespace com
} // namespace bl
