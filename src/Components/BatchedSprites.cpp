#include <BLIB/Components/BatchedSprites.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace com
{
BatchedSprites::BatchedSprites(rc::Renderer& renderer, const rc::res::TextureRef& texture,
                               unsigned int initialCapacity) {
    create(renderer, texture, initialCapacity);
}

void BatchedSprites::create(rc::Renderer& renderer, const rc::res::TextureRef& txtr,
                            unsigned int initialCapacity) {
    texture = txtr;
    buffer.create(renderer.vulkanState(), initialCapacity * 4, initialCapacity * 6);
    refreshTrans();
    drawParams = buffer.getDrawParameters();
}

void BatchedSprites::refreshTrans() {
    const bool wasTrans  = containsTransparency;
    containsTransparency = texture->containsTransparency();
    if (containsTransparency != wasTrans) { rebucket(); }
}

void BatchedSprites::updateDrawParams() {
    if (buffer.getIndexBuffer().vertexCount() > 0) {
        refreshTrans();
        drawParams = buffer.getDrawParameters();
        if (sceneRef.object) { syncDrawParamsToScene(); }
    }
}

} // namespace com
} // namespace bl
