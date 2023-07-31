#include <BLIB/Components/Slideshow.hpp>

namespace bl
{
namespace com
{
bool Slideshow::create(rc::vk::VulkanState& vs, const gfx::a2d::AnimationData& anim) {
    if (!gfx::a2d::AnimationData::isValidSlideshow(anim)) {
        BL_LOG_ERROR << "Animation is not a slideshow";
        return false;
    }

    if (indexBuffer.vertexCount() == 0) {
        indexBuffer.create(vs, 4, 6);
        indexBuffer.indices() = {0, 1, 2, 0, 2, 3};
        for (rc::prim::Vertex& vertex : indexBuffer.vertices()) {
            vertex.color = {1.f, 1.f, 1.f, 1.f};
        }
    }

    const sf::FloatRect src(anim.getFrame(0).shards.front().source);
    indexBuffer.vertices()[0].pos      = {0.f, 0.f, 0.f};
    indexBuffer.vertices()[0].texCoord = {src.left, src.top};
    indexBuffer.vertices()[1].pos      = {src.width, 0.f, 0.f};
    indexBuffer.vertices()[1].texCoord = {src.left + src.width, src.top};
    indexBuffer.vertices()[2].pos      = {src.width, src.height, 0.f};
    indexBuffer.vertices()[2].texCoord = {src.left + src.width, src.top + src.height};
    indexBuffer.vertices()[3].pos      = {0.f, src.height, 0.f};
    indexBuffer.vertices()[3].texCoord = {src.left, src.top + src.height};
    size                               = {src.width, src.height};

    indexBuffer.queueTransfer(rc::tfr::Transferable::SyncRequirement::Immediate);
}

} // namespace com
} // namespace bl
