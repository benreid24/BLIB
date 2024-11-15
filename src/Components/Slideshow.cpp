#include <BLIB/Components/Slideshow.hpp>

namespace bl
{
namespace com
{
bool Slideshow::create(rc::vk::VulkanState& vs, const Animation2DPlayer& anim) {
    if (!anim.getAnimation()->isSlideshow()) {
        BL_LOG_ERROR << "Animation is not a slideshow";
        return false;
    }

    if (indexBuffer.vertexCount() == 0) {
        indexBuffer.create(vs, 4, 6);
        indexBuffer.indices() = {0, 1, 2, 0, 2, 3};
        for (rc::prim::SlideshowVertex& vertex : indexBuffer.vertices()) {
            vertex.color = {1.f, 1.f, 1.f, 1.f};
        }
    }

    const sf::FloatRect src(anim.getAnimation()->getFrame(0).shards.front().source);
    indexBuffer.vertices()[0].pos = {0.f, 0.f, 0.f};
    indexBuffer.vertices()[1].pos = {src.width, 0.f, 0.f};
    indexBuffer.vertices()[2].pos = {src.width, src.height, 0.f};
    indexBuffer.vertices()[3].pos = {0.f, src.height, 0.f};
    for (rc::prim::SlideshowVertex& vertex : indexBuffer.vertices()) {
        vertex.slideshowIndex = anim.getPlayerIndex();
    }
    size       = {src.width, src.height};
    drawParams = indexBuffer.getDrawParameters();

    indexBuffer.queueTransfer(rc::tfr::Transferable::SyncRequirement::Immediate);
    return true;
}

bool Slideshow::setPlayer(const Animation2DPlayer& anim) {
    if (!anim.getAnimation()->isSlideshow()) {
        BL_LOG_ERROR << "Animation is not a slideshow";
        return false;
    }
    if (indexBuffer.vertexCount() == 0) {
        BL_LOG_ERROR << "Cannot set animation on slideshow before calling create()";
        return false;
    }

    for (rc::prim::SlideshowVertex& vertex : indexBuffer.vertices()) {
        vertex.slideshowIndex = anim.getPlayerIndex();
    }
    // TODO - handle size change?

    indexBuffer.insertBarrierBeforeWrite();
    indexBuffer.queueTransfer(rc::tfr::Transferable::SyncRequirement::Immediate);

    return true;
}

} // namespace com
} // namespace bl
